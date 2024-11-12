#!/usr/bin/env python
# coding: utf-8

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import csv from sklearn.datasets
import make_classification from xgboost
import XGBClassifier from sklearn.linear_model
import LogisticRegression from sklearn.tree
import DecisionTreeClassifier from sklearn.ensemble
import RandomForestClassifier from sklearn.metrics
import roc_curve from sklearn.metrics
import roc_auc_score from sklearn.metrics
import accuracy_score from sklearn.metrics
import cohen_kappa_score from sklearn.metrics
import recall_score from sklearn.metrics
import f1_score from sklearn.metrics
import precision_score from sklearn.metrics
import confusion_matrix from sklearn.metrics
import matthews_corrcoef from matplotlib
import pyplot
import scipy.stats as stats
import pickle
import random from tabulate import tabulate
import matplotlib.pyplot as plt\nplt.rcParams["figure.figsize"]=10,10')


random.seed(42)

dt_model = pickle.load(open("ml_models/dt.pkl", 'rb'))
lr_model = pickle.load(open("ml_models/lr.pkl", 'rb'))
rf_model = pickle.load(open("ml_models/rf.pkl", 'rb'))
xgb_model = pickle.load(open("ml_models/xgb.pkl", 'rb'))

test = pd.read_csv("table_test.csv", sep=',', index_col = 0)

Y_test = test['class']
X_test = test.drop(columns='class')
Y_test = Y_test.astype('category')


Y_test = np.array(Y_test)

dt_probs = dt_model.predict_proba(X_test)
lr_probs = lr_model.predict_proba(X_test)
rf_probs = rf_model.predict_proba(X_test)
xgb_probs = xgb_model.predict_proba(X_test)

ns_probs = [0 for _ in range(len(Y_test))]

# keep probabilities for the positive outcome only
dt_probs = dt_probs[:, 1]
lr_probs = lr_probs[:, 1]
rf_probs = rf_probs[:, 1]
xgb_probs = xgb_probs[:, 1]

# calculate scores
ns_auc = roc_auc_score(Y_test, ns_probs)
auc_dt = roc_auc_score(Y_test, dt_probs)
auc_lr = roc_auc_score(Y_test, lr_probs)
auc_rf = roc_auc_score(Y_test, rf_probs)
auc_xgb = roc_auc_score(Y_test, xgb_probs)

# calculate roc curves
dt_fpr, dt_tpr, _ = roc_curve(Y_test, dt_probs)
lr_fpr, lr_tpr, _ = roc_curve(Y_test, lr_probs)
rf_fpr, rf_tpr, _ = roc_curve(Y_test, rf_probs)
xgb_fpr, xgb_tpr, _ = roc_curve(Y_test, xgb_probs)
np_fpr, np_tpr, _ = roc_curve(Y_test, ns_probs)


pyplot.figure(figsize=(5,5))

pyplot.plot(dt_fpr, dt_tpr, label="DT auc: {:.3f}".format(auc_dt), linewidth=4.0)
pyplot.plot(lr_fpr, lr_tpr, label="LR auc: {:.3f}".format(auc_lr), linewidth=4.0)
pyplot.plot(rf_fpr, rf_tpr, label="RF auc: {:.3f}".format(auc_rf), linewidth=4.0)
pyplot.plot(xgb_fpr, xgb_tpr, label="XGB auc: {:.3f}".format(auc_xgb), linewidth=4.0)


pyplot.plot(np_fpr, np_tpr, label='No prediction', color="C7", linewidth=4.0)

# axis labels
pyplot.xlabel('False Positive Rate', fontsize = 16)
pyplot.ylabel('True Positive Rate', fontsize = 16)
# show the legend
pyplot.legend(fontsize=16, loc ="lower right", ncol=1)
pyplot.tick_params(axis='both', which='major', labelsize=16)
# displaying the title
plt.title("ROC curve", fontsize = 20)

plt.savefig('roc.png')
plt.close()


Y_pred_dt = dt_model.predict(X_test)
Y_pred_lr = lr_model.predict(X_test)
Y_pred_rf = rf_model.predict(X_test)
Y_pred_xgb = xgb_model.predict(X_test)

cm_dt =  confusion_matrix(Y_test, Y_pred_dt, labels=[0,1])
cm_lr =  confusion_matrix(Y_test, Y_pred_lr, labels=[0,1])
cm_rf =  confusion_matrix(Y_test, Y_pred_rf, labels=[0,1])
cm_xgb =  confusion_matrix(Y_test, Y_pred_xgb, labels=[0,1])

auc_dt = '%.2f'%(roc_auc_score(Y_test, Y_pred_dt))
acc_dt = '%.2f'%(accuracy_score(Y_test, Y_pred_dt))
sensitivity_dt =  '%.2f'%(cm_dt[0,0]/(cm_dt[0,0]+cm_dt[0,1]))
specificity_dt =  '%.2f'%(cm_dt[1,1]/(cm_dt[1,0]+cm_dt[1,1]))
mcc_dt =  '%.2f'%(matthews_corrcoef(Y_test, Y_pred_dt))

auc_lr = '%.2f'%(roc_auc_score(Y_test, Y_pred_lr))
acc_lr = '%.2f'%(accuracy_score(Y_test, Y_pred_lr))
sensitivity_lr =  '%.2f'%(cm_lr[0,0]/(cm_lr[0,0]+cm_lr[0,1]))
specificity_lr =  '%.2f'%(cm_lr[1,1]/(cm_lr[1,0]+cm_lr[1,1]))
mcc_lr = '%.2f'%(matthews_corrcoef(Y_test, Y_pred_lr))

auc_rf = '%.2f'%(roc_auc_score(Y_test, Y_pred_rf))
acc_rf = '%.2f'%(accuracy_score(Y_test, Y_pred_rf))
sensitivity_rf =  '%.2f'%(cm_rf[0,0]/(cm_rf[0,0]+cm_rf[0,1]))
specificity_rf =  '%.2f'%(cm_rf[1,1]/(cm_rf[1,0]+cm_rf[1,1]))
mcc_rf =  '%.2f'%(matthews_corrcoef(Y_test, Y_pred_rf))

auc_xgb = '%.2f'%(roc_auc_score(Y_test, Y_pred_xgb))
acc_xgb = '%.2f'%(accuracy_score(Y_test, Y_pred_xgb))
sensitivity_xgb =  '%.2f'%(cm_xgb[0,0]/(cm_xgb[0,0]+cm_xgb[0,1]))
specificity_xgb =  '%.2f'%(cm_xgb[1,1]/(cm_xgb[1,0]+cm_xgb[1,1]))
mcc_xgb = '%.2f'%(matthews_corrcoef(Y_test, Y_pred_xgb))


content1 = tabulate([
                ['DT', auc_dt, acc_dt, sensitivity_dt, specificity_dt, mcc_dt],
                ['LR', auc_lr, acc_lr, sensitivity_lr, specificity_lr, mcc_lr],
                ['RF', auc_rf, acc_rf, sensitivity_rf, specificity_rf, mcc_rf],
                ['XGB', auc_xgb, acc_xgb, sensitivity_xgb, specificity_xgb, mcc_xgb]
                 ],
               headers=['Model', 'AUC', 'Accuracy', 'Sensitivity', 'Specificity', 'MCC'], tablefmt="tsv")


with open('metrics.txt', 'w') as f:
    f.write(content1)