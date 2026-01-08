# -*- coding: utf-8 -*-
"""
Created on Wed Jan  7 22:06:07 2026

@author: jgive

Employee Data Exploration Tool
------------------------------

This program provides an interactive, menu‑driven interface for exploring and 
filtering a dataset of employee information. It uses a pandas DataFrame and 
allows the user to repeatedly sort, filter, and view the data until they choose 
to exit.

Features:
    • View the full employee dataset (name, department, salary)
    • Sort the data by:
        - name (ascending/descending)
        - department (ascending/descending)
        - salary (ascending/descending)
    • Filter by department:
        - Accepts multiple departments separated by spaces or commas
        - Validates input and returns only matching rows
        - Returns the filtered DataFrame for further operations
    • Filter by salary using strict mathematical parsing:
        - Supports <, >, <=, >=, min, max
        - Supports expressions like:
              <120000, 120000<, >=90000, 90000>=
        - Allows multiple conditions combined with "and"
        - Interprets expressions literally (strict math mode)
        - Builds lower/upper bounds and applies them to the DataFrame
    • Chained filtering:
        - Each filter updates the working DataFrame (current_df)
        - Users can apply multiple filters in sequence
    • Persistent menu loop:
        - The program continues running until the user types "exit"
        - Sorting and filtering always apply to the current filtered state

Overall:
    This script functions as a lightweight command‑line data browser, allowing 
    flexible exploration of employee information through sorting, department 
    filtering, and strict mathematical salary filtering.
"""

import pandas as pd
import re
#All data are fictious, any data correlating to real world data is pure coicidence. 
df = pd.DataFrame(
    data = {
        "name":["Alicia","Ben","Carla","Daniel","Evelyn", "Frank", "Grace", "Henry", "Isabella", "Jordan"],
        "department":["Engineering","HR","Engineering","Marketing","Engineering","Sales","Engineering","HR","Marketing","Engineering"],
        "salary":[120000,68000,95000,72000,105000, 88000, 99000, 65000, 110000, 87000]
        }
    )

current_df = df.copy()

sort_options = { 
    "1a": ("name", True), 
    "1b": ("name", False), 
    "2a": ("department", True), 
    "2b": ("department", False),
    "3a": ("salary", True), 
    "3b": ("salary", False) }

def filter_departments(current_df):
    userInput_dept = input("Enter which departments you would only like to see. Options -> Engineering, Marketing, Sales, HR (Use a space or comma to separate options): ")
    cleaned_UserInput_dept = userInput_dept.replace(","," ")
    userInput_dept_choice = cleaned_UserInput_dept.split()
    filtered_dept = current_df[current_df["department"].isin(userInput_dept_choice)]
   
    if filtered_dept.empty:
        return print("That is not a valid option")
    
    print(filtered_dept)
    
    return filtered_dept

def filter_salary(current_df):
    lower = None
    upper = None
    
    userInput_salary = input("Enter how you want to filter the salary. Use the following key terms or symbols -> min, max, >, >=, <, <=. Use the key word 'and' to combine filtering options (ex: 95000 < and > 120000): ")
   
    inputted_text = userInput_salary.lower()
    inputted_text = inputted_text.replace(",", " ")
    inputted_text = inputted_text.replace("and", "&")
    inputted_text = " ".join(inputted_text.split())
    
    expressions = re.findall(r'\d+\s*[<>]=?\s*|\s*[<>]=?\s*\d+', inputted_text) 
    expressions = [expr.replace(" ", "") for expr in expressions]
    
    numbers = [int(n) for n in inputted_text.split() if n.isdigit()]
    
    if "min" in inputted_text and numbers:
        lower = numbers[0]
    
    if "max" in inputted_text and numbers:
        upper = numbers[-1]
        
    for expr in expressions:
        expr = expr.replace(" ", "") 
        
        # <=number 
        if expr.startswith("<=") and expr[2:].isdigit(): 
            upper = int(expr[2:]) 
        
        # number<= 
        elif expr.endswith("<=") and expr[:-2].isdigit(): 
            lower = int(expr[:-2]) 
        
        # <number 
        elif expr.startswith("<") and expr[1:].isdigit(): 
            upper = int(expr[1:]) 
        
        # number< 
        elif expr.endswith("<") and expr[:-1].isdigit(): 
            lower = int(expr[:-1]) 
        
        # >=number 
        elif expr.startswith(">=") and expr[2:].isdigit():
            lower = int(expr[2:]) 
            
        # number>= 
        elif expr.endswith(">=") and expr[:-2].isdigit(): 
            upper = int(expr[:-2]) 
        
        # >number 
        elif expr.startswith(">") and expr[1:].isdigit(): 
            lower = int(expr[1:]) 
        
        # number> 
        elif expr.endswith(">") and expr[:-1].isdigit(): 
            upper = int(expr[:-1])
    
    condition = pd.Series([True] * len(df)) 
    
    if lower is not None: 
        condition &= current_df["salary"] > lower 
    
    if upper is not None: 
        condition &= current_df["salary"] < upper 
    
    filtered = current_df[condition]
    
    print(filtered)
    return filtered

filter_options = {
    #1 -> just filter departments
    "1": filter_departments,
    #2 -> just filter salary
    "2": filter_salary,
    }

while True:
    userInput_1 = input("Choose your action (Input the number) -> 1) Print Dataframe 2) Sort Dataframe 3) Filter Dataframe: ")
    if userInput_1 == "1":
        print(current_df[["name","department","salary"]])

    elif userInput_1 == "2":
    
        userInput_column = input("Choose your column (Input the number) -> 1a) Name (Ascending) 1b) Name (Descending) 2a) Department (Ascending) 2b) Department (Descending) 3a) Salary (Ascending) 3b) Salary (Descending): ")
    
        if userInput_column in sort_options:
            column, direction = sort_options[userInput_column]
            current_df = current_df.sort_values(column, ascending=direction)
            print(current_df)
    
        else:
            print(userInput_column + " is not a listed option. Please select an option from the list. Program shutting down, restart to try again.")
        
    
    elif userInput_1 == "3":
        userInput_column_filter = input("Choose your column (Input the number) -> 1) Department  2) Salary ")
        
        if userInput_column_filter in filter_options:
            current_df = filter_options[userInput_column_filter](current_df)
        
        else:
            print(userInput_column_filter + " is not a listed option. Please select an option from the list. Program shutting down, restart to try again.")
    
    elif userInput_1 == "exit":
        break
    
    else:
        print(userInput_1 + " is not a listed option. Please select an option from the list. Program shutting down, restart to try again.")

