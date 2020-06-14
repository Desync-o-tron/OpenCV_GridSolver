#include "stdafx.h"
#include "Generic_Grid_Functions.h"
#include "ErrorChecking.h"

void gridErrorChecking(vector <RectStats>& gridNums)
{
    /*
    for each set of duplicate numbers remaining
    determine which of the duplicates is a worse match(lowest confidence)
    take that number, and compare it against the remaining possible numbers that have not been assigned
    whichever number it is most similar to, assign that one
    continue through the sets until all numbers have been assigned
    */

    vector<int> numsNotPresent;

    for (int i = 1; i < 10; i++)
        numsNotPresent.push_back(i);
    for (int i = 0; i < 9; i++)
        numsNotPresent.erase(remove(numsNotPresent.begin(), numsNotPresent.end(), gridNums[i].guessedNum), numsNotPresent.end());

    //what we have left over in numsNotPresent is obviously, *the numbers not present in the grid.*
    if (numsNotPresent.size() == 0)
        return;

    cout << "\nnums not present\n";
    for (int i = 0; i < numsNotPresent.size(); i++)
        cout << numsNotPresent[i] << " ";
    cout << endl;

    sort(gridNums.begin(), gridNums.end(),
        [](RectStats const& a, RectStats const& b) { return a.guessedNum < b.guessedNum; });

    //mark the dupliacte field where applicable
    int priorNum = -1;
    for (int i = 0; i < 9; i++) {
        if (priorNum == gridNums[i].guessedNum) {
            gridNums[i].duplicate = true;
            gridNums[i - 1].duplicate = true;
        }
        priorNum = gridNums[i].guessedNum;
    }

    //find the duplicates
    vector <RectStats> duplicateGridNums;
    for (int i = 0; i < 9; i++)
        if (gridNums[i].duplicate == true)
            duplicateGridNums.push_back(gridNums[i]);

    //sort the duplicates by confidence, high to low
    sort(duplicateGridNums.begin(), duplicateGridNums.end(),
        [](RectStats const& a, RectStats const& b) { return a.confidences[0] > b.confidences[0]; });

    cout << "duplicates, confidence\n";
    for (int i = 0; i < duplicateGridNums.size(); i++)
        cout << duplicateGridNums[i].guessedNum << ", " << duplicateGridNums[i].confidences[0] << endl;
    cout << endl;

    // find all the numbers of each subset with the highest confidence.
    // aka which numbers come first in the set & delete them from duplicate grid numbers 
    //int prevNum = -1;
    //for (int i = 0; i < duplicateGridNums.size();)
    //{
    //    bool iterate = false;
    //    if (prevNum != duplicateGridNums[i].guessedNum)
    //        duplicateGridNums.erase(duplicateGridNums.begin() + i);
    //    else
    //        iterate = true;

    //    prevNum = duplicateGridNums[i].guessedNum;

    //    if (iterate)
    //        ++i;
    //}
    // find all the numbers of each subset with the highest confidence.
    // aka which numbers come first in the set & delete them from duplicate grid numbers 
    int prevNum = -1;
    for (int i = 0; i < duplicateGridNums.size();)
    {
        if (prevNum != duplicateGridNums[i].guessedNum) {
            duplicateGridNums.erase(duplicateGridNums.begin() + i);
            continue;
        }
        i++;
    }

    cout << "weeded out duplicates\n";
    for (int i = 0; i < duplicateGridNums.size(); i++)
        cout << duplicateGridNums[i].guessedNum << " ";
    cout << endl;

    // take the things with the highest confidence and match them with the numbers not present
    //TODO comment is gay
    for (int i = 0; i < duplicateGridNums.size(); i++) {
        int greatestConfidence = -1;

        for (int j = 0; j < numsNotPresent.size(); j++)

            if (greatestConfidence < duplicateGridNums[i].confidences[numsNotPresent[j] - 1])
            {
                greatestConfidence = duplicateGridNums[i].confidences[numsNotPresent[j] - 1];
                duplicateGridNums[i].guessedNum = numsNotPresent[j];
            }

        numsNotPresent.erase(std::remove(numsNotPresent.begin(), numsNotPresent.end(), duplicateGridNums[i].guessedNum), numsNotPresent.end());
    }

    /*
    cout << "\nduplicates list filled with NNP\n";
    for (int i = 0; i < duplicateGridNums.size(); i++)
    cout << duplicateGridNums[i].guessedNum << " ";
    cout << endl;
    */

    //now duplicate grid numbers is only filled with legit duplicates.

    for (int j = 0; j < duplicateGridNums.size(); j++) {

        for (int i = 0; i < gridNums.size(); i++) {

            if (gridNums[i].gridPos == duplicateGridNums[j].gridPos)
            {
                //cout << "changed grid pos " << gridNums[i].gridPos << " from a " << gridNums[i].guessedNum;
                gridNums[i] = duplicateGridNums[j];
                //cout << " to a" << gridNums[i].guessedNum << endl;
                break;
            }
        }
    }

    sort(gridNums.begin(), gridNums.end(),
        [](RectStats const& a, RectStats const& b) { return a.gridPos < b.gridPos; });

}


vector<vector<int>> generateGridGuesses(const vector<vector<int>> & inputSets, vector<int> positions) {
    int inputLen = inputSets[0].size() - 1;
    vector<vector<int>> outputPossibilities{};


    vector<int> possibleOutput{};
    for (int i = 0; i < inputSets.size(); i++)
        possibleOutput.push_back(inputSets[i][positions[i]]);

    outputPossibilities.push_back(possibleOutput);

    if (positions[0] == 1 && positions[1] == 2 && positions[2] == 2) {
        int ti = 2;
    }
    //each call produces one more of these
    //000
    //001
    //002
    //...
    //010
    //011
    //...
    //333
    bool finishedGeneration = false;
    int i = 0;
    while (1) 
    {
        //i < (positions.size() - 1) || positions[i] < depth

        if (positions[i] < inputLen) 
        {
            positions[i]++;
            break;
        }
        else if (i < positions.size()){
            if (i == positions.size() - 1)
            {
                finishedGeneration = true;
                break; //max,...max
            }
            positions[i] = 0;
            i++;
        }
        //else if (i == positions.size() && positions[i] == inputLen) {
        //else { 
        //    finishedGeneration = true;
        //    break; //max,...max
        //}
    }


    //for (int i = 0; i < positions.size(); i++) {
    //    cout << positions[i] << " ";
    //}
    //cout << endl;
    
    if (finishedGeneration) {
        return outputPossibilities;
    }
    
    auto newPossibilities = generateGridGuesses(inputSets, positions);
    outputPossibilities.insert(outputPossibilities.end(), newPossibilities.begin(), newPossibilities.end());
    return outputPossibilities;
}