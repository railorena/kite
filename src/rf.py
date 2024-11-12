#!/usr/bin/env python3
# coding: utf-8

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import csv
import time
from tabulate import tabulate
from sklearn.model_selection import train_test_split
from sklearn.feature_selection import SelectFromModel
from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier
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

# ## Random Forest classification parameters
model = RandomForestClassifier()

param_grid = {
      'max_depth': [7, 15, 20],
      'n_estimators': [150, 300, 500],
      'min_samples_leaf': [10, 15, 20],
      'criterion': ['gini', 'entropy'],
      'max_features': ['sqrt', 'log2'],
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
rf_od = grid_search.best_estimator_
rf_od.fit(X_train,Y_train)

# ## Saving best parameters
pkl_filename1 = "ml_models/rf.pkl"
with open(pkl_filename1, 'wb') as file:
    pickle.dump(rf_od, file)
