# Fetch the repos given a particular CSV class list

import csv
import os
import argparse

parser = argparse.ArgumentParser(description='Fetch All Repos')

parser.add_argument('csv', type=str, help='The CSV file containing the class list')
parser.add_argument('dir', type=str, help='The directory to clone the repositories into')
parser.add_argument('--count', type=int, help='Maximum count to process', default=50)

args = parser.parse_args()

# Check that the location for cloning exists

if not os.path.exists(args.dir):
    print('Error: Directory for cloning repos does not exist')
    exit(-1)

# Open up the CSV file
if not os.path.exists(args.csv):
    print('Error: CSV file does not exist')
    exit(-1)

problemIDs = []
countDone = 0

with open(args.csv, 'r') as csvfile:
   reader = csv.DictReader(csvfile)

   for row in reader:
      # Bail out if we have already processed enough
      if countDone >= args.count:
         break

      name = row['Name']
      netid = row['NetID']
      repourl = row['RepositoryURL']

      # Ignore blank lines
      if netid == None or repourl == None or len(name) == 0:
         continue

      countDone += 1

      dirLoc = os.path.join(args.dir, 'cse30264-sp25-' + netid)
      print(dirLoc)

      repoInfo = repourl.split('/')

      if len(repoInfo) < 2:
         print('No repo listed for ', netid)
         problemIDs.append(netid)
      else:
         repoName = repoInfo[-2] + '/' + repoInfo[-1]

         if not os.path.exists(os.path.join(dirLoc)):
            print('Cloning: ', netid, ' ', repoName)
            print('URL: ', repourl)

            # Clone the repository
            result = os.system('git -C ' + args.dir + ' clone git@github.com:' + repoName)

            if result != 0:
               print('Error: Unable to clone repository for netid: ', netid)
               problemIDs.append(netid)
         else:
            print('Pulling: ', netid, ' ', repourl)

            # Clone the repository
            result = os.system('git -C ' + os.path.join(args.dir, dirLoc) + ' pull')


for theID in problemIDs:
    print('Problem with: ', theID)
