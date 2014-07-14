# __BatClassify__

BatClassify automates the extraction and classification of echolocation calls (for a subset of UK bat species) from time-expansion and direct-sampling detector recordings.

## Getting started (Windows)
-----------------------
### Step 1. Download
Find 'BatClassify_win32.zip' at https://bitbucket.org/chrisscott/batclassify/downloads 
Right click and select 'Save link as...' to download the file. Unzip the file contents to a location of your choice.

### Step 2. Run BatClassify
The BatClassify.exe is found in the 'bin' folder inside the 'BatClassify' folder previously extracted. Double-click the .exe file to bring up the BatClassify user interface. 

If the recordings to be analysed were made with a time-expansion detector and are long continuous files, go to step 3. If the recordings are from a direct-sampling detector go straight to step 4.

### Step 3. Split time-expansion recordings
Continuous time-expansion recordings must be broken up into smaller files prior to analysis. The 'split' process extracts non-silent regions of the recording, saving each as a separate file.

Click the 'Split' tab of the user interface. Select the folder containing the recording(s) to be split using the directory browser (click 'Browse...'). Next select the folder where the output files should be saved, as above. Click run. If a large number of files were selected for splitting, a progress bar will indicate the percentage of work complete.

### Step 4. Analyse recordings
Select the Analyse tab of the user interface. Select the folder containing the recording(s) to be analysed using the directory browser (click 'Browse...'). Next select the folder where the output files should be saved, as above. Select whether spectrograms of extracted calls should be saved using the check box. Click run. If a large number of files were selected for splitting, a progress bar will indicate the percentage of work complete.

### Step 5. View results
Once analysis has finished, the 'Results.csv' file will be found in the output folder selected in step 4. CSV files can be viewed in a number of programs e.g. Microsoft Excel, OpenOffice and Google Docs. Analysis results are organised with a row for each recording, and a column for each species. Classifier output for each species or species group is a number between 0 and 1, and represents an estimate of presence within that recording. For example, a recording containing calls from a lesser horseshoe only should have values near 0 for all species except _Rhip_, where values closer to 1 indicate increasing confidence in presence. Note that multiple species may be present in a single recording, in which case more than one value in that row may be non-zero.

## Methods
-----------------------
* [libsndfile](http://www.mega-nerd.com/libsndfile/) is used to read audio files
* Audio is internally resampled to 500 kHz for consistency using [libsndfile](http://www.mega-nerd.com/SRC/)
* Time domain audio samples converted to frequency domain spectrogram using [fftw](http://www.fftw.org/)
* Spectrogram background subtraction using a local average of pixel intensity
* Blob detection via [connected component labelling](http://www.iis.sinica.edu.tw/papers/fchang/1362-F.pdf)
* Feature extraction - spectral and temporal moments, gradient histogram
* Classification using a [tree-based ensemble](http://orbi.ulg.ac.be/bitstream/2268/9357/1/geurts-mlj-advance.pdf)

## Evaluation
-----------------------
Precision-Recall metric to evaluate the quality of the output of the classifiers:

| Species   | Precision | Recall |  N  |
|:---------:|:---------:|:------:|:---:|
| Bbar		| 0.9955    | 0.9095 | 243 |
| Malc    	| 0.8300    | 0.8700 | 23  |
| Mbec    	| 1.0000    | 0.6250 | 16  |
| Mbra/Mmys | 0.8611    | 0.9511 | 244 |
| Mdau     	| 0.9944    | 0.8786 | 210 |
| Mnat     	| 0.9771    | 0.9624 | 133 |
| NSL      	| 0.9974    | 0.9872 | 391 |
| Paur     	| 0.9831    | 0.8788 | 198 |
| Ppip     	| 0.9901    | 0.9804 | 510 |
| Ppyg     	| 0.9644    | 0.9675 | 308 |
| Rfer     	| 1.0000    | 1.0000 | 79  |
| Rhip     	| 1.0000    | 1.0000 | 353 |

NSL group comprises _Nyctalus noctula_, _Nyctalus leisleri_ and _Eptesicus serotinus_.

Notice not all UK species are covered yet, and sample sizes for _Myotis alcathoe_ and _Myotis bechsteinii_ are currently small.

Pettersson D240X and D500X recordings were used to train/test models.

## Credits
-----------------------
* BCT
* DEFRA (http://www.defra.gov.uk/)
* PTES (http://www.ptes.org/)
* University of Leeds

## Contact
-----------------------
Maintainer: Chris Scott (fbscds@gmail.com).

## License
-----------------------
Licensed under the [GPL, Version 3](http://www.gnu.org/copyleft/gpl.html)
