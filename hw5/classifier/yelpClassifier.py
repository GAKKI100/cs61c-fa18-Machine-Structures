import sys
import math

# Star bins used for classifying Yelp reviews
STARS = [1.0, 3.0, 5.0]

class YelpClassifier(object):

    def __init__(self):
        self.NUM_REVIEWS = {}   # {num_stars : # of reviews with num_stars}
        self.NUM_WORDS = {}     # {num_stars : # of words total across all reviews with num_stars}
        self.LIKELIHOODS = {}   # {word : P(word | num_stars)}
        self.PRIORS = {}        # {num_stars : P(num_stars)}

    ######################################################
    ################# DRIVER FUNCTIONS ##################
    ######################################################

    def train(self, train_rdd):
        self.NUM_REVIEWS, self.NUM_WORDS = self.calculate_num_reviews_and_words_per_num_stars(train_rdd)
        self.PRIORS = self.calculate_priors()
        self.LIKELIHOODS = self.calculate_likelihoods(train_rdd)

    def classify(self, test_rdd):
        predictions = self.classify_reviews(test_rdd)
        predictedReviews = predictions.collect()
        return predictedReviews

    ######################################################
    ################# PYSPARK FUNCTIONS ##################
    ######################################################
    
    def calculate_likelihoods(self, train_rdd):

        # Given the reviews in train_rdd, calculates P(word | num_stars) for every word found in a review,
        # i.e. the likelihood of a word in a review given the number of stars that review received.
        # The likelihood is calculated as, across all of the reviews given NUM_STARS:
        # P(WORD | NUM_STARS) = # of occurences of WORD / # of total words in all reviews with NUM_STARS

        # Transformations:
        # 1. (review_id, num_stars, review_text_as_string) --> [((num_stars, word_in_review_1), 1), ((num_stars, word_in_review_2), 1), ...]
        # 2. ((num_stars, word_in_review), 1), ((num_stars, word_in_review), 1) --> ((num_stars, word_in_review), num_reviews_of_num_stars_with_word)
        # 3. ((num_stars, word_in_review), num_reviews_with_word_and_stars) --> (num_stars, {word : probability_in_review_of_num_stars})
        # 4. (num_stars, {word1 : prob1, word2 : prob2, word3 : prob3...})


        class_likelihoods = train_rdd \
                            .flatMap(self.review_to_word_counts) \
                            .reduceByKey(self.add_review_counts) \
                            .map(self.counts_to_probabilities) \
                            .aggregateByKey({}, self.combine_probability_tables,  self.combine_probability_tables)

        LIKELIHOODS = {}
        for num_stars, likelihood in class_likelihoods.collect():
            LIKELIHOODS[int(num_stars)] = likelihood

        return LIKELIHOODS

    def calculate_num_reviews_and_words_per_num_stars(self, train_rdd):

        # Calculate number of reviews per number of stars

        # Transformations:
        # 1. (review_id, num_stars, review_text_as_string) --> (num_stars, (1, num_words))
        # 2. (num_stars, (1, num_words)), (num_stars, (1, num_words)) --> (num_stars, (num_reviews_of_num_stars, num_words_total_of_num_stars))


        num_reviews_words_per_num_stars = train_rdd \
                                             .map(self.review_to_num_stars_num_words) \
                                             .reduceByKey(self.add_review_and_word_counts) 


        NUM_REVIEWS = {}
        NUM_WORDS = {}
        for num_stars, counts in num_reviews_words_per_num_stars.collect():
            num_reviews, num_words = counts
            NUM_REVIEWS[int(num_stars)] = int(num_reviews)
            NUM_WORDS[int(num_stars)] = int(num_words)

        return NUM_REVIEWS, NUM_WORDS

    # Calculates priors for each class as: P(num_stars) = # of reviews with num_stars / # of total reviews 
    def calculate_priors(self):
        total_num_reviews = 0
        for num_stars, num_reviews in self.NUM_REVIEWS.items():
            total_num_reviews += num_reviews

        PRIORS = self.NUM_REVIEWS.copy()
        for num_stars, num_reviews in PRIORS.items():
            prior = float(num_reviews) / float(total_num_reviews)
            PRIORS[num_stars] = prior

        return PRIORS

    def classify_reviews(self, test_rdd):

        # Transformations:
        # 1. (review_id, num_stars, review_text_as_string) --> [((num_stars1, review_id) word1), ((num_stars1, review_id), word2), ((num_stars2, review_id), word1)...]
        # 2. ((num_stars1, review_id), word1) -> ((num_stars1, review_id), log_p_word1)
        # 3. ((num_stars1, review_id) p_word1), ((num_stars1, review_id), p_word2) --> ((num_stars1, review_id), p_word_sum)
        # 4. ((num_stars1, review_id), log_likelihood) --> ((num_stars1, review_id), log_posterior)
        # 5. ((num_stars1, review_id), log_posterior) --> (review_id, (num_stars1, log_posterior1)
        # 6. (review_id, (num_stars1, log_posterior1)), (review_id, (num_stars2, log_posterior2)) --> (review_id, (num_stars, max_posterior)

        predictions = test_rdd \
                            .flatMap(self.review_to_num_stars_and_word_pairs) \
                            .map(self.words_to_log_likelihoods) \
                            .reduceByKey(self.add_log_likelihoods) \
                            .map(self.likelihood_to_posterior) \
                            .map(self.review_id_only_as_key) \
                            .reduceByKey(self.find_max_posterior) \
                            .sortByKey()

        return predictions

    ######################################################
    ################# HELPER FUNCTIONS  ##################
    ######################################################

    # ___________________________________________________ #
    # ____________TRAINING HELPERS_______________________ #
    # ___________________________________________________ #

    # ____________calculate_likelihoods() helpers________ #

    @staticmethod
    def review_to_word_counts(review):

        # Maps the words in a review to pairs of that word, the number of stars
        # of the review that the word was in, and the count of that word.

        # (review_id, num_stars, review_text_as_string) --> [((num_stars, word_in_review_1), 1), ((num_stars, word_in_review_2), 1), ...]
        num_stars = review[1]
        review_string = review[2]
        words = review_string.split(" ")
        my_list = []	
        for word in words:
            info = ((num_stars, word), 1)
            my_list.append(info)
        return my_list
        
    @staticmethod
    def add_review_counts(count1, count2):



        # Adds two review counts together

        # ((num_stars, word_in_review), 1), ((num_stars, word_in_review), 1) --> ((num_stars, word_in_review), num_reviews_of_num_stars_with_word)
        return count1 + count2

    def compute_likelihood(self, count_of_word, num_stars):

        # Helper function to compute the likelihood of a word with Laplace smoothing

        probability = float((count_of_word + 1)) / float((self.NUM_WORDS[num_stars] + 1))
        return probability  
    
    def counts_to_probabilities(self, num_stars_and_word_counts):

        # Maps the count of a word, over reviews of the same number of stars,
        # to its likelihood: P(WORD | NUM_STARS) = count of WORD / count of ALL words in reviews with NUM_STARS

        # ((num_stars, word_in_review), num_reviews_with_word_and_stars) --> (num_stars, {word : probability_in_review_of_num_stars})
        num_stars = num_stars_and_word_counts[0][0]
        word = num_stars_and_word_counts[0][1]
        count_of_word = num_stars_and_word_counts[1]
        probability = self.compute_likelihood(count_of_word, int(num_stars))
        return (num_stars, {word : probability})
        

    def combine_probability_tables(self, word1_and_probability, word2_and_probability):

        # Given two probability tables as dictionaries (e.g. {word1: prob1, word2: prob2...}),
        # Combines the two tables, by adding the contents of the second table into the first,
        # and returning the now updated first table.

        word1_and_probability.update(word2_and_probability)
        return word1_and_probability

    # ____________calculate_num_reviews_per_num_stars() helpers________ #
    
    @staticmethod
    def review_to_num_stars_num_words(review):

        # Converts a review into a pair of its number of stars with the
        # number of words in the review and the number of reviews it represents

        # (review_id, num_stars, review_text_as_string) --> (num_stars, (1, num_words))

        num_stars = review[1]
        review_string = review[2]
        words = review_string.split(" ")
        num_words = len(words) 
        return (num_stars, (1, num_words))

    @staticmethod
    def add_review_and_word_counts(count1, count2):

        # Combines the values of two pairs outputted by review_to_num_stars_num_words

        # (num_stars, (1, num_words)), (num_stars, (1, num_words)) --> (num_stars, (num_reviews_of_num_stars, num_words_total_of_num_stars))

        return (count1[0] + count2[0], count1[1] + count2[1])  

    # _________________________________________________________ #
    # ____________CLASSIFICATION HELPERS_______________________ #
    # _________________________________________________________ #

    # ____________classify_reviews() helpers____________________ #

    @staticmethod
    def review_to_num_stars_and_word_pairs(review):

        # For all words in the review and all possible star ratings for the review,
        # returns list of pairs of the possible number of stars, the review's ID, and the word itself.

        review_id, review_text = review
        review = review_text.split(" ")

        possible_num_stars_word_pairs = []
        for word in review:
            possible_num_stars_and_word = [((num_stars, review_id), word) for num_stars in STARS]
            possible_num_stars_word_pairs.extend(possible_num_stars_and_word)

        return possible_num_stars_word_pairs

    def words_to_log_likelihoods(self, stars_id_word):

        # Given a possible number of stars, maps a word to its log-likelihood using self.LIKELIHOODS
        # i.e. for WORD and possible NUM_STARS, maps WORD to log(P(WORD | NUM_STARS))
        # If WORD is not found in self.LIKELIHOODS, then it has zero likelihood, which after Laplace smoothing,
        # maps to a log-likelihood = log(1 / # of total words in reviews with NUM_STARS + 1).

        num_stars, review_id = int(stars_id_word[0][0]), stars_id_word[0][1]
        word = stars_id_word[1]

        likelihoods = self.LIKELIHOODS[num_stars]

        if word in likelihoods:
            word_likelihood = likelihoods[word]
        else:
            num_words_with_num_stars = self.NUM_WORDS[num_stars]
            word_likelihood = 1.0 / float(num_words_with_num_stars + 1)
        log_likelihood = math.log(word_likelihood)

        return ((num_stars, review_id), log_likelihood)

    @staticmethod
    def add_log_likelihoods(likelihood1, likelihood2):

        # Adds two log-likelihoods together

        return float(likelihood1) + float(likelihood2)

    def likelihood_to_posterior(self, stars_id_lhood):

        # Given a likelihood, P(WORD | NUM_STARS), calculates the posterior:
        # P(WORD, NUM_STARS) = P(WORD | NUM_STARS) * P(NUM_STARS)
        # P(NUM_STARS) is stored in self.PRIORS

        num_stars, review_id = int(stars_id_lhood[0][0]), stars_id_lhood[0][1]
        log_likelihood = float(stars_id_lhood[1])
        prior = self.PRIORS[num_stars]
        posterior = log_likelihood + math.log(prior)

        return ((num_stars, review_id), posterior)

    @staticmethod
    def review_id_only_as_key(stars_id_posterior):

        # Modifies key-value pair such that the key only contains the review_id
        # All other "values" in the key are moved into the value of the pair

        num_stars, review_id = stars_id_posterior[0][0], stars_id_posterior[0][1]
        posterior = stars_id_posterior[1]
        
        return (review_id, (num_stars, posterior))

    @staticmethod
    def find_max_posterior(num_stars_posterior1, num_stars_posterior2):

        # Returns the max of two posterior probabilities as well as the number of stars
        # rating the corresponds to the greater posterior probability

        # (review_id, (num_stars1, log_posterior1)), (review_id, (num_stars2, log_posterior2)) --> (review_id, (num_stars, max_posterior))
        log_posterior1 = num_stars_posterior1[1]
        log_posterior2 = num_stars_posterior2[1]
        if log_posterior1 > log_posterior2:
            return num_stars_posterior1
        return num_stars_posterior2
        
                

        


