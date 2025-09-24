#!/usr/bin/env python3
# coding: utf-8

import pandas as pd
import numpy as np
import pickle
import shap
import random

random.seed(42)

xgb_model = pickle.load(open("output/xgb.pkl", 'rb'))
train = pd.read_csv("output/train_data.csv", sep=',', index_col = 0)
X_train = train.drop(columns='class')

explainer = shap.Explainer(xgb_model, feature_names=X_train.columns)
shap_values = explainer(X_train)


values_s = shap_values.values
feature_names = X_train.columns
xgb_resultX = pd.DataFrame(values_s, columns=feature_names)
vals = np.abs(xgb_resultX.values).mean(0)
shap_importance = pd.DataFrame(
    list(zip(feature_names, vals)),
    columns=['col_name', 'feature_importance_vals']
)
shap_importance = shap_importance[shap_importance['feature_importance_vals'] != 0]
shap_importance.sort_values(
    by=['feature_importance_vals'],
    ascending=False,
    inplace=True
)

shap_importance.to_csv("output/important_kmers.csv",index=False)
