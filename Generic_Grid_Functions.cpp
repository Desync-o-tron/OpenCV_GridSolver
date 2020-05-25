#include "stdafx.h"
#include "Generic_Grid_Functions.h"

void gridErrorChecking2(vector <RectStats> & gridNums)
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



// for handwriting
void gridErrorChecking(vector <gridNum> & gridNums, bool confidence)
{
    /*
    for each set of duplicate numbers remaining
    determine which of the duplicates is a worse match(lowest confidence)
    take that number, and compare it against the remaining possible numbers that have not been assigned
    whichever number it is most similar to, assign that one
    continue through the sets until all numbers have been assigned
    */

    vector<int> numsNotPresent;

    for (int i = 0; i < 9; i++)
        numsNotPresent.push_back(i + 1);
    for (int i = 0; i < 9; i++)
        numsNotPresent.erase(remove(numsNotPresent.begin(), numsNotPresent.end(), gridNums[i].guessedNum), numsNotPresent.end());

    //what we have left over in numsNotPresent is obviously, *the numbers not present in the grid.*
    if (numsNotPresent.size() == 0)
        return;

    sort(gridNums.begin(), gridNums.end(),
        [](gridNum const& a, gridNum const& b) { return a.guessedNum < b.guessedNum; });

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
    vector <gridNum> duplicateGridNums;
    for (int i = 0; i < 9; i++)
        if (gridNums[i].duplicate == true)
            duplicateGridNums.push_back(gridNums[i]);


    //sort the duplicates by confidence, high to low
    if (confidence == true)
        sort(duplicateGridNums.begin(), duplicateGridNums.end(),
            [](gridNum const& a, gridNum const& b) { return a.confidence > b.confidence; });


    // find all the numbers of each subset with the highest confidence.
    // aka which numbers come first in the set & delete them from duplicate grid numbers 
    int prevNum = -1;
    for (int i = 0; i < duplicateGridNums.size();)
    {
        bool iterate = false;
        if (prevNum != duplicateGridNums[i].guessedNum) {
            duplicateGridNums.erase(duplicateGridNums.begin() + i);
        }
        else
            iterate = true;

        prevNum = duplicateGridNums[i].guessedNum;

        if (iterate)
            ++i;
    }

    if (confidence == true) 
    {
        // take the things with the highest confidence and match them with the numbers not present
        for (int i = 0; i < duplicateGridNums.size(); i++) {
            float greatestConfidence = -1;

            for (int j = 0; j < numsNotPresent.size(); j++)

                if (greatestConfidence < duplicateGridNums[i].confidence[numsNotPresent[j] - 1])
                {
                    greatestConfidence = duplicateGridNums[i].confidence[numsNotPresent[j] - 1];
                    duplicateGridNums[i].guessedNum = numsNotPresent[j];
                }

            numsNotPresent.erase(std::remove(numsNotPresent.begin(), numsNotPresent.end(), duplicateGridNums[i].guessedNum), numsNotPresent.end());
        }
    }
    else //just assign missing nums without prejuidice
    {
        for (int i = 0; i < duplicateGridNums.size(); i++) {
            duplicateGridNums[i].guessedNum = numsNotPresent.back();
            if (numsNotPresent.size() == 0)
                cout << "NUMSNOTPRESENTERROR\n";
            else 
                numsNotPresent.pop_back();
        }
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
        [](gridNum const& a, gridNum const& b) { return a.gridPos < b.gridPos; });

}


bool display_Img(Mat frame)
{
    if (!frame.data) {
        printf("No image data \n");
        return  false;
    }

    static int num_images_shown;
    static int xOffset, yOffset;

    std::string imgID = "No." + std::to_string(num_images_shown);

    namedWindow(imgID, CV_WINDOW_FREERATIO);
    //namedWindow(imgID, CV_WINDOW_NORMAL);
    moveWindow(imgID, xOffset, yOffset);

    imshow(imgID, frame);

    num_images_shown++;
    xOffset += 40;
    yOffset += 40;
    return true;
}
