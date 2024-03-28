from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import confusion_matrix
from sklearn.ensemble import RandomForestClassifier
from sklearn.svm import LinearSVC
from sklearn.linear_model import LogisticRegression


def KNN_model(X_train, X_test, y_train, y_test):
    knn = KNeighborsClassifier(n_neighbors=7)
    knn.fit(X_train, y_train)
    y_pred = knn.predict(X_test)
    print('KNN confusion_matrix:')
    print(confusion_matrix(y_test, y_pred))
    return knn


def RFC_model(X_train, X_test, y_train, y_test):
    rfc = RandomForestClassifier(random_state=21)
    rfc.fit(X_train, y_train)
    y_pred = rfc.predict(X_test)
    print('Random Forest confusion_matrix:')
    print(confusion_matrix(y_test, y_pred))
    return rfc


def LSVC_model(X_train, X_test, y_train, y_test):
    lsvc = LinearSVC(dual='auto', loss='hinge',
                     max_iter=10000, random_state=21)
    lsvc.fit(X_train, y_train)
    y_pred = lsvc.predict(X_test)
    print('LinearSVC confusion_matrix:')
    print(confusion_matrix(y_test, y_pred))
    return lsvc


def LR_model(X_train, X_test, y_train, y_test):
    lr = LogisticRegression(max_iter=10000, random_state=21)
    lr.fit(X_train, y_train)
    y_pred = lr.predict(X_test)
    print('Logistic Regression confusion_matrix:')
    print(confusion_matrix(y_test, y_pred))
    return lr
