from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
import pandas as pd
import requests
import numpy as np
import pandas as pd
from datetime import datetime, timedelta
from statsmodels.tsa.arima.model import ARIMA
from sklearn.metrics import *
from math import sqrt
date_format_str = '%Y-%m-%d %H:%M:%S'

data = requests.get(
    "https://api.thingspeak.com/channels/1576707/feeds.json?results=100")


def pull_data():
    element = data.json()
    row = element['feeds']
    LPG = []
    CO = []
    date = []
    for i in row:
        date.append(i['created_at'])
        LPG.append(i['field1'])
        CO.append(i['field2'])
    arr = np.stack((date, LPG, CO), axis=1)
    df = pd.DataFrame(arr)
    df = df.rename({0: 'date', 1: 'LPG', 2: 'CO'}, axis=1)
    # df = df.astype({'LPG': float, 'CO': float})
    return df


def Data(DataFrame):
    data = DataFrame
    print(data)
    data['date'] = data['date'].str[:-1]
    for i in range(len(data['date'])):
        data['date'][i] = data['date'][i].replace("T", " ")
    print(data)

    data1 = data
    length = len(data1.date)-1
    # print(length)

    for i in range(length):
        start = datetime.strptime(data1.date[i], date_format_str)
        end = datetime.strptime(data1.date[i+1], date_format_str)
        diff = end - start
        if (diff.total_seconds() > 330):

            add = start + timedelta(seconds=319)
            add = add.strftime(date_format_str)
            line = pd.DataFrame({"date": add, "LPG": float(
                "NaN"), "CO": float("NaN"), }, index=[i+1])
            data1 = pd.concat(
                [data1.iloc[:i+1], line, data1.iloc[i+1:]]).reset_index(drop=True)
            length += 1
            continue

    data1.index = data1['date']
    data1.index.name = None
    data = data1
    return data


# fill missing (NaN) by Interpolation method
def FillMissingData(DataFrame):
    data = DataFrame
    data = data.astype({'LPG': float, 'CO': float})
    data = data.interpolate(method='linear', limit_direction='forward')

    return data


def ARIMA_agorithm(CleanData):
    data = CleanData
    X = data['LPG'].values
    size = int(len(X) * 0.66)
    train, test = X[0:size], X[size:len(X)]
    history = [x for x in train]
    predictions = list()
    OBS = []
    YHAT = []
    # walk-forward validation
    for t in range(len(test)):
        model = ARIMA(history, order=(1, 1, 1))
        model_fit = model.fit()
        output = model_fit.forecast()
        yhat = output[0]
        predictions.append(yhat)
        obs = test[t]
        history.append(obs)
        OBS.append(obs)
        YHAT.append(yhat)
        df = pd.DataFrame(np.stack((YHAT, OBS), axis=1),
                          columns=['predicted', 'expected'])
        print('predicted=%f, expected=%f' % (yhat, obs))
    # evaluate forecasts
    rmse = sqrt(mean_squared_error(test, predictions))
    print('Mean Absolute Error:', mean_absolute_error(test, predictions))
    print('Mean Squared Error:', mean_squared_error(test, predictions))
    print('Test RMSE: %.3f' % rmse)

    return df


def LR_Algo_for_CO(column, Fill_data):
    # data forecast from ARIMA
    LPG_pred = column['predicted'].values

    # data from fill missing data
    lpg = Fill_data['LPG'].values.reshape(
        len(Fill_data), 1)
    co = Fill_data['CO'].values

    # split data 80-20
    X_train, X_test, y_train, y_test = train_test_split(
        lpg, co, test_size=0.2, random_state=0)
    regressor = LinearRegression()

    # fit model
    regressor.fit(X_train, y_train)
    print('intercept:', regressor.intercept_)
    print('slope:', regressor.coef_)

    # the regression model
    CO_pred = regressor.intercept_ + regressor.coef_ * LPG_pred

    print('coefficient of determination:', regressor.score(X_train, y_train))
    print(CO_pred)
    return CO_pred


if __name__ == "__main__":

    data = Data(DataFrame=pull_data())
    fill = FillMissingData(data)
    LPG = ARIMA_agorithm(fill)
    print(LPG)
    print(LR_Algo_for_CO(LPG, fill))
