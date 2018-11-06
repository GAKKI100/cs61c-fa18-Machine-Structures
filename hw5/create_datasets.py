import sys
import json
import re
import os
import time

if __name__ == '__main__':

    dataset_sizes = {'sample': (40, 10), 'small': (1280, 320), 'medium': (32000, 8000), 'large': (2000000, 400000)}
    dataset_name = sys.argv[1]
    num_train, num_test = dataset_sizes[dataset_name]

    os.chdir('yelp_dataset')
    os.system('head -n ' + str(num_train + num_test) + ' yelp_academic_dataset_review.json > temp.json')
    os.system('mv temp.json ..')
    os.chdir('..')

    with open('temp.json', 'r') as f:
        dataset = f.read().split('\n')
        dataset.remove('')

    print("Done loading in dataset")

    try:
        os.mkdir('datasets')
    except:
        pass
    os.chdir('datasets')
    try:
        os.mkdir(dataset_name)
    except:
        pass
    os.chdir(dataset_name)

    train = open('train.txt', 'w')
    test = open('test.txt', 'w')

    reviews = [json.loads(review) for review in dataset]

    count = 1
    total = len(reviews)
    for review in reviews:
        review_id = review['review_id']

        num_stars = review['stars']
        if num_stars > 4.0:
            num_stars =  5
        elif num_stars > 2.0:
            num_stars =  3
        else:
            num_stars =  1

        review_text = review['text']
        review_text = re.sub(r'[^\w\s]','', review_text).replace("\n", "").replace("\r", "").lower()
        if num_train > 0:
            train.write(str(review_id) + ' ' + str(num_stars) + ' ' + str(review_text))
            train.write('\n')
            num_train -= 1
        elif num_test > 0:
            test.write(str(review_id) + ' ' + str(num_stars) + ' ' + str(review_text))
            test.write('\n')
            num_test -= 1

        bar_len = 30
        filled_len = int(round(bar_len * count / float(total)))

        percents = round(100.0 * count / float(total), 1)
        bar = '=' * filled_len + '-' * (bar_len - filled_len)

        sys.stdout.write("\r[%s] %s%s ... %d/%d reviews created" % (bar, percents, '%', count, total))
        sys.stdout.flush()

        count += 1

    sys.stdout.write("\n")

    train.close()
    test.close()
    os.chdir('../../')
    os.system('rm -f temp.json')
