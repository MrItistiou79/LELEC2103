import pickle
from numpy import newaxis

def load_model(model_path):
    return pickle.load(open(model_path, 'rb'))

class Model:
    def save(self, model_path):
        pickle.dump(self, open(model_path, 'wb'))

    def eval(self, melvecs):
        if (melvecs.ndim==1):
            return self.predict(melvecs[:,newaxis])  #MAYBE INVERT :,newaxis
        return self.predict(melvecs)

    def train(self, X_train, y_train):
        # To overwrite by ML model
        return

    def predict(self, melvecs):
        # To overwrite by ML model
        return None

from sklearn.neighbors import KNeighborsClassifier

class KNNModel(Model):
    def __init__(self, K):
        self.model = KNeighborsClassifier(n_neighbors=K, weights='uniform', algorithm='auto', metric='minkowski') # Create the classifier

    def train(self, X_train, y_train):
        self.model.fit(X_train, y_train)

    def predict(self, melvecs):
        print(melvecs.shape)
        melvecs = melvecs.flatten()
        return self.model.predict(melvecs[newaxis,:])

