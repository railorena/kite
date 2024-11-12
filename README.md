# kite

## Count module
```
./kite count -i [filename].csv -t 8
```
    -i input csv file with samples name and conditions
    -t number of threads
*./kite count -i input_training.csv -t 8*

## Selection module
```
./kite selection -i [filename].csv -p 60
```
    -i input csv file with samples name and conditions
    -p percentage for k-mer frequency [60, 70, 80]

*./kite selection -i input_training.csv -p 80*


If running the selection module without the count module, a file called matrix.tsv with the k-mer counting is needed. The matrix should be separated by space.


## Training module
```
./kite training -i [filename].csv -t 8 -m s
```
    -i input csv file with samples name and conditions
    -t number of threads
    -m machine learning models [s, c, a] (s=simple models, c=complex models, a=all)

*./kite training -i input_training.csv -t 8 -m s*

## Prediction module
```
./kite prediction -i [filename].csv -t 8 -m s
```
    -i input csv file with samples name and conditions
    -t number of threads
    -m machine learning models [s, c, a] (s=simple models, c=complex models, a=all)

*./kite prediction -i input_test.csv -t 8 -m s*
