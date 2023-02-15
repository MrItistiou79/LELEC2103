import numpy as np

from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import train_test_split, StratifiedKFold
import pickle
import matplotlib.pyplot as plt
import os, sys

from classify import dataset, modelling

from seaborn import heatmap
from sklearn.metrics import confusion_matrix
def show_confusion_matrix(y_predict, y_true2, classnames, title=""):
    """
    From target and prediction arrays, plot confusion matrix.
    The arrays must contain ints.
    """

    confmat = confusion_matrix(
        y_true2, y_predict, labels=np.arange(np.max(y_true2) + 1)
    )
    heatmap(
        confmat.T,
        square=True,
        annot=True,
        fmt="d",
        cbar=False,
        xticklabels=classnames,
        yticklabels=classnames,
    )
    plt.xlabel("True label")
    plt.ylabel("Predicted label")
    plt.title(title)
    plt.show()
    return None
def get_accuracy(prediction, target):
    """
    Compute the accuracy between prediction and ground truth.
    """

    return np.sum(prediction == target) / len(prediction)
    

def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)

if len(sys.argv) < 2:
    print("No dataset provided, usage: python knn.py [path/to/Dataset_ESC-50]")
    sys.exit(1)

dataset_path = dir_path(sys.argv[1])

"Parameters"
K = 10 # Number of neighbours
sel_class = [12,14,40,41,49] # Select only some classes for the classification
#dataset_path = os.path.join(os.path.dirname(os.getcwd()), dataset_path)


allclassnames, allpath_mat = dataset.gen_allpath(dataset_path)
allpath_sel = np.array([allpath_mat[idx,:] for idx in sel_class])
classnames = np.array([allclassnames[idx] for idx in sel_class])
sel_class_ids = np.arange(len(sel_class))
all_sel_class_ids = np.repeat(sel_class_ids, 40)
data_path = allpath_sel.reshape(-1)

print('The selected classes are {}'.format(classnames))

# ----------------------------------------------------------------------------------
"""Creation of the dataset"""
myds = dataset.SoundDS(all_sel_class_ids, duration=750, shift_pct=0.0, data_path=data_path, allpath_mat=allpath_mat, normalize=True)

"Random split of 70:30 between training and validation"
train_pct = 0.7

featveclen = np.prod(myds[0][0].shape) # number of items in a feature vector
num_items = len(myds) # number of sounds in the dataset
num_sounds = len(myds.allpath_mat[0,:]) # number of sounds in each class
num_classes = int(num_items/num_sounds) # number of classes
num_learn = round(num_sounds * train_pct) # number of sounds among num_sounds for training

data_aug_factor = 1
sel_class_ids = np.arange(len(sel_class))
sel_class_ids_aug = np.repeat(sel_class_ids, num_sounds*data_aug_factor)

"Compute the matrixed dataset, this takes some seconds, but you can then reload it by commenting this loop and decommenting the np.load below"
X = np.zeros((data_aug_factor*num_classes*num_sounds, featveclen))
for s in range(data_aug_factor):
    for i, featvec in enumerate(myds):
        X[s*num_classes*num_sounds+i,:] = featvec[0].reshape(-1)
np.save("feature_matrix_2D.npy", X)

# X = np.load("feature_matrix_2D.npy")

"Labels"
y = sel_class_ids_aug.copy()
# ----------------------------------------------------------------------------------

"""Model definition and training"""

knn = modelling.KNNModel(K)

"Shuffle then split the dataset into training and testing subsets"
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, stratify=y) # random_state=1

print(X_test.shape)

knn.train(X_train, y_train)

# save the trained model 
filename = 'KNN.pickle'
knn.save(filename)

pickle.dump(knn.model, open('KNN2.pickle', 'wb'))

prediction_knn = knn.model.predict(X_test)
accuracy_knn = get_accuracy(prediction_knn, y_test)
print('Accuracy of KNN with fixed train/validation sets : {:.1f}%'.format(100*accuracy_knn))
show_confusion_matrix(prediction_knn, y_test, classnames)