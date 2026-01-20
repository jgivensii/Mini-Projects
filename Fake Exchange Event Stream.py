# -*- coding: utf-8 -*-
"""
Created on Mon Jan 19 06:49:28 2026

@author: jgive

This script creates a lightweight “fake exchange” by streaming intraday MSFT market data through a queue 
and processing each row as if it were a live market event. It begins by loading and preparing the dataset,
parsing timestamps, sorting the index, normalizing column names, and converting percentage‑change values into decimals. 
Several empty DataFrames are initialized to store daily, weekly, monthly, and rolling statistics that are computed later in the script.

A `Daily` class handles the first layer of aggregation. It resamples the intraday data into daily open, high, low, close, 
volatility, and mean values, storing them in a shared `msft_daily` DataFrame. This daily structure becomes the foundation 
for the `Weekly` and `Monthly` classes, which further resample the data into weekly and month‑end intervals. These classes 
compute their own OHLC and volatility values, building a multi‑timeframe view of the market. A `Rolling` class then calculates 
20‑period rolling statistics such as mean, high, low, standard deviation, volatility, and z‑scores, which are commonly used in 
quantitative analysis and signal generation.

To visualize the incoming data, the script defines a `Daily_plt` function that updates a real‑time Matplotlib chart. 
It extracts a sliding window of recent OHLC data, converts the index into a plottable format, and updates a persistent figure 
with the latest close prices. The chart automatically rescales and refreshes as new data arrives, creating the effect of a live price feed.

The event‑driven behavior comes from a queue. Each row of the DataFrame is pushed into the queue in chronological order, 
simulating a stream of market ticks. The event loop repeatedly pulls the next item from the queue, prints it, determines its true 
index in the DataFrame, and updates the real‑time plot. A short delay between events mimics the pacing of a live feed. Together, 
these components form a simplified but functional model of an event‑driven trading engine, complete with data ingestion, time‑ordered 
processing, multi‑timeframe analytics, and incremental visualization.


"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from queue import Queue as que
import time


file_Path = 'C:/Users/jgive/Documents/Python Projects/Mini Projects/Data/msft_intraday-30min_historical-data-03-17-2025.csv'
df = pd.read_csv(file_Path, index_col = 0, parse_dates = True)
df.sort_index(ascending=True, inplace=True)
msft_daily = pd.DataFrame()
msft_wkly = pd.DataFrame()
msft_monthly = pd.DataFrame()
msft_rolling = pd.DataFrame()
plt.ion()

df['%Chg'] = pd.to_numeric(df['%Chg'].str.replace('%',''))/100
df = df.rename(columns=str.lower)

class Daily:
    
    def __init__(self, df):
        self.df = df
        self.msft_daily = pd.DataFrame()
        
    def Open(self):
        daily_open = df['open'].resample('D').first()
        daily_open.index = daily_open.index.to_period('D')
        msft_daily['open'] = daily_open
        return daily_open
        
    def High(self):
        daily_high = df['high'].resample('D').max()
        daily_high.index = daily_high.index.to_period('D')
        msft_daily['high'] = daily_high
        return daily_high
        
    def Low(self):
        daily_low = df['low'].resample('D').min()
        daily_low.index = daily_low.index.to_period('D')
        msft_daily['low'] = daily_low
        return daily_low
        
    def Close(self):
        daily_close = df['last'].resample('D').last()
        daily_close.index = daily_close.index.to_period('D')
        msft_daily['close'] = daily_close
        return daily_close
        
    def Volatility(self):
        daily_volatility = df['%chg'].resample('D').std()
        daily_volatility.index = daily_volatility.index.to_period('D')
        msft_daily['volatility'] = daily_volatility
        return daily_volatility
    
    def Mean(self, msft_daily_df):
        msft_mean = (msft_daily_df['open']+msft_daily_df['close'])/2
        msft_daily['mean'] = msft_mean
        return msft_mean

daily = Daily(df)
daily.Open(); daily.High(); daily.Low(); daily.Close(); daily.Volatility()    
daily.msft_daily = daily.msft_daily.ffill()
df = df.rename(columns=str.lower) 
df['close'] = df['last']

class Weekly:
    def Open(self):
        wkly_open = daily.Open().resample('W').first()
        msft_wkly['open'] = wkly_open
        return wkly_open
        
    def High(self):
        wkly_high = daily.High().resample('W').max()
        msft_wkly['high'] = wkly_high
        return wkly_high

    def Low(self):
        wkly_low = daily.Low().resample('W').min()
        msft_wkly['low'] = wkly_low
        return wkly_low
        
    def Close(self):
        wkly_close = daily.Close().resample('W').last()
        msft_wkly['close'] = wkly_close
        return wkly_close
    
    def Volatility(self):
        wkly_volatility = daily.Volatility().resample('W').std()
        msft_wkly['volatility'] = wkly_volatility
        return wkly_volatility
    
    msft_wkly
    
    
class Monthly:
    def Open(self):
        monthly_open = daily.msft_daily['open'].resample('ME').first()
        msft_monthly['open'] = monthly_open
        return monthly_open

    def High(self):
        monthly_high = daily.msft_daily['high'].resample('ME').max()
        msft_monthly['high'] = monthly_high
        return monthly_high
    
    def Low(self):
        monthly_low = daily.msft_daily['Low'].resample('ME').min()
        msft_monthly['low'] = monthly_low
        return monthly_low
        
    def Close(self):
        monthly_close =  daily.msft_daily['close'].resample('ME').last()
        msft_monthly['close'] = monthly_close
        return monthly_close

    def Volatility(self):
        monthly_volatility = daily.msft_daily['volatility'].resample('ME').std()
        msft_monthly['volatility'] = monthly_volatility
        return monthly_volatility

    msft_monthly

class Rolling:
    rolling_return = daily.Mean(msft_daily).pct_change(fill_method = None)
    msft_rolling['mean'] = daily.Mean(msft_daily).rolling(window=20).mean()
    msft_rolling['high'] = daily.High().rolling(window=20).max()
    msft_rolling['low']  = daily.Low().rolling(window=20).min()
    msft_rolling['std'] = daily.Mean(msft_daily).rolling(20).std()
    msft_rolling['volatility'] = rolling_return.rolling(window = 20).std()
    msft_rolling['zscore_20'] = ( (daily.Mean(msft_daily) - msft_rolling['mean']) / msft_rolling['std'] )

def Daily_plt(df,i,window):
   start = max(0, i - window + 1)
   plt_Daily = df[['open','high','low','close']].iloc[start:i+1]
   
   if plt_Daily.empty:
        return

   if isinstance(plt_Daily, pd.Series):
        plt_Daily = plt_Daily.to_frame().T


   x = plt_Daily.index.to_numpy()
   y = plt_Daily['close'].values

   # Create figure once
   if not hasattr(Daily_plt, "fig"):
        Daily_plt.fig, Daily_plt.ax = plt.subplots(figsize=(10,5))
        Daily_plt.line, = Daily_plt.ax.plot([], [], lw=2, color='blue')
        Daily_plt.marker, = Daily_plt.ax.plot([], [], 'o', color='red')

   ax = Daily_plt.ax
   line = Daily_plt.line
   marker = Daily_plt.marker

   line.set_data(x, y)
   marker.set_data([x[-1]], [y[-1]])

   ax.relim()
   ax.autoscale_view()
   ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M'))
   plt.setp(ax.get_xticklabels(), rotation=45, ha='right')

   Daily_plt.fig.canvas.draw()
   Daily_plt.fig.canvas.flush_events()



   
line = que()
print(df.columns)

for i in range(len(df)):
    line.put(df.iloc[i])

   
while not line.empty():
    current = line.get()
    time.sleep(0.5)
    print(current)

    # find REAL index position
    i = df.index.get_loc(current.name)

    Daily_plt(df, i, 50)
    

