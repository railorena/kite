#!/usr/bin/env python3
# coding: utf-8

import pandas as pd
from sklearn.model_selection import GridSearchCV, StratifiedKFold
from xgboost import XGBClassifier
import pickle
import random
import warnings

warnings.filterwarnings("ignore")

# ## Setting seed
random.seed(42)

# ## Importing the data
train = pd.read_csv("output/train_data.csv", sep=',', index_col = 0)

# ## Splitting in features and classes
X_train = train.drop(columns='class')
Y_train = train['class']
cond = Y_train.value_counts()
Y_train = Y_train.astype('category')

# ## eXtreme Gradient Boosting classification parameters
model = XGBClassifier(
    objective= 'binary:logistic'
)

param_grid = {
    'max_depth': range (7, 15, 20),
    'n_estimators': range(150, 300, 500),
    'learning_rate': [0.01, 0.1, 0.2, 0.3],
    'scale_pos_weight': [1, 10, 25, 50, 75,  100]
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
xgb_od = grid_search.best_estimator_
xgb_od.fit(X_train,Y_train)

# ## Saving best parameters
pkl_filename1 = "output/xgb.pkl"
with open(pkl_filename1, 'wb') as file:
    pickle.dump(xgb_od, file)
