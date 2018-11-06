import sys
import subprocess

from classifier.yelpClassifier import YelpClassifier
from pyspark import SparkContext
from optparse import OptionParser

######################################################
################# RDD CREATOR FUNCTIONS ##################
######################################################

def initRDDs(training_data_file, test_data_file):
    sc = SparkContext()

    # takes text file of reviews --> RDD
    try:
        train_rdd = sc.textFile(training_data_file).flatMap(input_line_to_review)
        test_rdd = sc.textFile(test_data_file).flatMap(input_line_to_review_no_label)
        actual_rdd = sc.textFile(test_data_file).flatMap(input_line_to_review)
        return train_rdd, test_rdd, actual_rdd
    except:
        raise Exception("Unable to load train and test data files")
        return

# Assuming each line of the input txt file is in the format:
# ID NUM_STARS review_text
# 
# Parses line of input txt file and emits tuples (review_id, num_stars, review_text)
# Example:
# 1bnj890_W2 5 This restaurant was terrible I can't believe it was this bad...
def input_line_to_review(input_line):
    # some reviews may be malformed/not contain all of the fields
    tokens = input_line.split(" ", 2)
    review_id, num_stars, review_text = tokens[0], tokens[1], tokens[2]
    return [(review_id, num_stars, review_text)]

# Assuming each line of the input txt file is in the format:
# ID NUM_STARS review_text
# 
# Parses line of input txt file and emits tuples (review_id, review_txt)
# Example:
# 1bnj890_W2 5 This restaurant was terrible I can't believe it was this bad...
def input_line_to_review_no_label(input_line):
    # some reviews may be malformed/not contain all of the fields
    tokens = input_line.split(" ", 2)
    review_id, review_text = tokens[0], tokens[2]
    return [(review_id, review_text)]


# returns the actual star label for a given review
def get_review_stars(actual_rdd):
    # Transformations:
    # 1. [(review_id, num_stars, review_text_as_string)] --> [(review_id, true_num_stars)]

    reviews_and_stars = actual_rdd \
                        .flatMap(filter_only_review_id_and_stars) \
                        .sortByKey()

    return reviews_and_stars

# Converts a review to a key-value pair of only review ID and its number of stars
def filter_only_review_id_and_stars(review):
    review_id, num_stars, review_text = review
    return [(review_id, num_stars)]

if __name__ == "__main__":

    parser = OptionParser()

    parser.add_option("-t", "--test", default="hive", help="Specify where you are testing. Options are 'local' and 'hive'.")
    parser.add_option("-d", "--dataset", default="sample", help="Specify which dataset to test on. Options are 'sample', 'small', 'medium', 'large'")

    (options, args) = parser.parse_args()

    if options.test == "local":
        training_data_file = "./datasets/%s/train.txt" % options.dataset
        test_data_file = "./datasets/%s/test.txt" % options.dataset
    elif options.test == "hive":
        training_data_file = "/home/tmp/cs61c/hw5/datasets/%s/train.txt" % options.dataset
        test_data_file = "/home/tmp/cs61c/hw5/datasets/%s/test.txt" % options.dataset
    else:
        parser.print_help()
        raise Exception('Incorrect command line arguments found, please refer above for the proper syntax')

    # Initialize the RDD's
    # Training RDD contains training data tuples of (review_id, num_stars, review_text)
    # Testing RDD contains testing data tuples of (review_id, review_text)
    # Actual RDD contains testing data tuples of (review_id, num_stars, review_text)
    train_rdd, test_rdd, actual_rdd = initRDDs(training_data_file, test_data_file)

    # Run the driver functions
    # classifier.train runs on the training RDD
    # classifier.classify runs on the testing RDD
    classifier = YelpClassifier()
    classifier.train(train_rdd)
    predictedReviews = classifier.classify(test_rdd)

    # Print debugging output if using sample dataset
    if (options.dataset == "sample"):
        f = open('./sample_test/your_debug_output.txt', 'w')
        f.write("NUM_REVIEWS dictionary:\n")
        f.write(str(classifier.NUM_REVIEWS))
        f.write("\n\nNUM_WORDS dictionary:\n")
        f.write(str(classifier.NUM_WORDS))
        f.write("\n\nPRIORS dictionary:\n")
        f.write(str(classifier.PRIORS))
        f.write("\n\nLIKELIHOODS dictionary:\n")
        f.write(str(classifier.LIKELIHOODS))
        f.write("\n\nCLASSIFIER OUTPUT:\n")
        f.write(("PREDICTED RESULT (review_id, predicted_label, total_log_likelihood):"
            "\t\tACTUAL RESULT (review_id, actual_label):\n"))

    # summarize results
    actualReviews = get_review_stars(actual_rdd).collect()
    hits = 0
    star_hits_dict = {1: 0, 3: 0, 5: 0}
    total = 0
    star_total_dict = {1: 0, 3: 0, 5: 0}

    for predicted, actual in zip(predictedReviews, actualReviews):
        if (options.dataset == "sample"):
            f.write(str(predicted) + "\t\t\t\t")
            f.write(str(actual) + "\n")
        actualLabel = int(actual[1])
        predictedLabel = int(predicted[1][0])
        if actualLabel == predictedLabel:
            star_hits_dict[actualLabel] += 1
            hits += 1
        star_total_dict[actualLabel] += 1
        total += 1

    accuracy = float(hits) / float(total)
    one_star_acc = float(star_hits_dict[1]) / float(star_total_dict[1])
    three_star_acc = float(star_hits_dict[3]) / float(star_total_dict[3])
    five_star_acc = float(star_hits_dict[5]) / float(star_total_dict[5])

    # Compare debugging output if using sample dataset
    if options.dataset == "sample":
        f.close()
        if options.test == "local":
            staff_output = "./sample_test/staff_debug_output_local.txt"
        else:
            staff_output = "./sample_test/staff_debug_output_hive.txt"
        try:
            subprocess.check_output(["diff", "-u", staff_output, "./sample_test/your_debug_output.txt"])
            print("\n\n\n\t\t\t\t\tYOUR SAMPLE OUTPUT MATCHES THE STAFF SAMPLE OUTPUT!")
        except subprocess.CalledProcessError as e:
            print(("\n\n\n\t\t\t\t\tYOUR SAMPLE OUTPUT DOES NOT MATCH THE STAFF SAMPLE OUTPUT. "
                "YOU CAN VIEW THE DIFFS IN debug_diffs.txt"))
            diff_file = open("debug_diffs.txt", "w")
            diff_file.write(e.output)
            diff_file.close()

    print("\n\n\n\t\t\t\t\t############################################################")
    print("\t\t\t\t\t\t\t\t\t\t\t\t")
    print("\t\t\t\t\t\tOVERALL ACCURACY: %f (%d/%d)\t\t" %(accuracy, hits, total))
    print("\t\t\t\t\t\t1 STAR REVIEW ACCURACY: %f (%d/%d)\t" \
        %(one_star_acc, star_hits_dict[1], star_total_dict[1]))
    print("\t\t\t\t\t\t3 STAR REVIEW ACCURACY: %f (%d/%d)\t" \
        %(three_star_acc, star_hits_dict[3], star_total_dict[3]))
    print("\t\t\t\t\t\t5 STAR REVIEW ACCURACY: %f (%d/%d)\t" \
        %(five_star_acc, star_hits_dict[5], star_total_dict[5]))
    print("\t\t\t\t\t\t\t\t\t\t\t\t")
    print("\t\t\t\t\t############################################################\n\n\n\n")
