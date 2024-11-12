#!/usr/bin/env python3
# coding: utf-8

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import csv
from tabulate import tabulate
from sklearn.model_selection import train_test_split
from sklearn.feature_selection import SelectFromModel
from sklearn.model_selection import GridSearchCV
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import StratifiedKFold
import pickle
import random
import warnings

warnings.filterwarnings("ignore")


# ## Setting seed
random.seed(42)

# ## Importing the data
train = pd.read_csv("train_data.csv", sep=',', index_col = 0)

# ## Splitting in features and classes
X_train = train.drop(columns='class')
Y_train = train['class']
cond = Y_train.value_counts()
Y_train = Y_train.astype('category')

# ## Logistic Regression classification parameters
model = LogisticRegression()

param_grid = {  'C': [1, 10, 100, 1000],
                'penalty': ['none','l1', 'l2'],
                'max_iter': list(range(150, 300, 500)),
                'solver': ['newton-cg', 'lbfgs', 'liblinear', 'sag', 'saga']
             }

n_samples = min(cond)

if n_samples < 10:
    grid_search = GridSearchCV( estimator=model,
                                 param_grid=param_grid,
                                 cv=StratifiedKFold(n_samples),
                                 n_jobs = 1, verbose = 0,
                                 scoring = 'accuracy')
else:
    grid_search = GridSearchCV( estimator=model,
                                 param_grid=param_grid,
                                 cv=StratifiedKFold(10),
                                 n_jobs = 1, verbose = 0,
                                 scoring = 'accuracy')


# ## Fitting the grid search
grid_search.fit(X_train, Y_train)
lr_od = grid_search.best_estimator_
lr_od.fit(X_train,Y_train)

# ## Saving best parameters
pkl_filename1 = "ml_models/lr.pkl"
with open(pkl_filename1, 'wb') as file:
    pickle.dump(lr_od, file)
