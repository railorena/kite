# KITE
[![Kmtricks](https://img.shields.io/badge/Kmtricks-documentation-blue)](https://github.com/tlemane/kmtricks)


## Description

KITE (K-mer Integration for Transcriptomic Exploration) is a bioinformatics pipeline for 
differential k-mer analysis from RNA-seq data. It provides modules for k-mer counting, 
filtering, machine learning methods, and a full end-to-end workflow.

## Installation

There are three ways to install KITE: download a pre-built apptainer/singularity container, 
create a container from a definition file, or install the software via Miniconda.

### Method 1: Download apptainer container

Prerequisite: apptainer/singularity

```
apptainer pull --arch amd64 library://devbio2m/kite/kite:latest
mv kite_latest.sif kite.sif
```

### Method 2: Create container

Prerequisite: apptainer/singularity

```
git clone https://github.com/Bio2M/KITE
cd KITE
apptainer build kite.sif kite.def
```

### Method 3: Miniconda

Prerequisites: 

* miniconda installed and operational
* bc installed (shell calculator)


```
git clone https://github.coms.Bio2M/KITE
cd KITE
conda create -f kite_env.yml
export PATH=${PWD}/src:$PATH
```

## Features

* **kcount** – Generate k-mer occurrence matrices from RNA-seq fastq samples using `kmtricks`.
* **kfilter** – Select differential k-mers based on a threshold.
* **kml** – Train machine learning model and get the most important k-mers.
* **pipeline** – Run kCount → kFilter → kML in a single command.


## Modules

| Module     | Description                                                             |
| ---------- | ----------------------------------------------------------------------- |
| `kcount`   | Run k-mer counting. Generates a k-mer occurrence matrix.                |
| `kfilter`  | Run k-mer filtering. Selects differential k-mers based on threshold.    |
| `kml`      | Run machine learning methods and get the most important k-mers.         |
| `pipeline` | Run full pipeline: `kcount` → `kfilter` → `kml`.                        |


### Options

| Option         | Description                                                          |
| -------------- | ------------------------------------------------------------------   |
| `-i`           | Sample metadata.                                                     |
| `-p`           | Selection threshold for k-mer filtering [0.4–0.7] (default=0.5).     |
| `-t`           | Number of threads (default=4).                                       |
| `-d`           | Directory containing fastq samples.                                  |
| `-m`           | Directory for k-mer matrix (default: output/output.tsv)              |
| `-s`           | Separator for k-mer matrix [s=space, t=tab, c=comma] (default: s).   |
| `-h`, `--help` | Show help message.                                                   |


## Usage

### With apptainer

**input files are in the working directory or HOME directory**

```
export PATH=/path/to/kite-container-directory:$PATH

kite.sif kcount -i metadata.csv -n 12 -d path/to/fastq
kite.sif kfilter -i metadata.csv -n 4 -s t -t 0.5
kite.sif kml -i metadata.csv -n 4
```

**input files are not in the working directory or the HOME directory**

```
apptainer run --bind /path/to/files  path/to/kite.sif kcount -i metadata.csv -n 12 -d path/to/fastq
apptainer run --bind /path/to/files  path/to/kite.sif kfilter -i metadata.csv -n 4 -s t -t 0.5
apptainer run --bind /path/to/files  path/to/kite.sif kml -i metadata.csv -n 4
```

### With miniconda

```
conda activate kite 

kite kcount -i metadata.csv -n 12 -d path/to/fastq
kite kfilter -i metadata.csv -n 4 -s t -t 0.5
kite kml -i metadata.csv -n 4
```

## Input
* `[metadata.csv]` – file with sample metadata (-i option)
* `[path/to/fastq]` – directory for metadata (-d option)
* `[matrix.tsv]` – k-mer count matrix (-m option, for kfilter module)


## Output

All output files are stored in the `output/` directory:

* `matrix.tsv` – k-mer count matrix
* `matrix_kf.tsv` – filtered k-mer matrix
* `qvalue.tsv` – selected k-mers with q-values
* `train_data.csv` – table to train XGB model
* `xgb.pkl` – XGB model
* `important_kmers` – importance of k-mers by SHAP


## Notes

* The recommended threshold for `kfilter` is between 0.4 and 0.7, although users may choose more stringent (0.9) or more relaxed (0.1) thresholds.
* `kml` will not run if the number of k-mers is too low (<10) or too high (>1,000,000). Adjust threshold accordingly.
* Ensure your input file and fastq directory are correctly formatted.
* The `-m` option is exclusively for cases where kfilter is executed with an externally generated matrix. It should not be used during standard pipeline execution.
