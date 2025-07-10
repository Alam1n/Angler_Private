import pandas as pd
import operator
import joblib
import time
import uuid
import sys
import os
import io
import json
import numpy as np
print("Import successfully")
from sklearn.preprocessing import MinMaxScaler, StandardScaler, RobustScaler, StandardScaler


class NotebookExporter:
    def __init__(self):
        self.data_exploration_cells = []
        self.preprocessing_cells = []
        self.model_cells = []
        self.prediction_cells = []

    def add_data_exploration(self, filename):
        code_str = f"""
import pandas as pd

df = pd.read_csv("{filename}")

print(df.head())

print(df.info())
"""
        self._append_cell(self.data_exploration_cells, code_str)

    def add_preprocessing(self, code_str):
        self._append_cell(self.preprocessing_cells, code_str)

        
    def add_Visualization(self, code_str):
        self._append_cell(self.prediction_cells, code_str)

    def add_model(self, code_str):
        self._append_cell(self.model_cells, code_str)

    def add_prediction(self, code_str):
        self._append_cell(self.prediction_cells, code_str)

    def _append_cell(self, section_list, code_str):
        cell = {
            "cell_type": "code",
            "metadata": {},
            "source": [code_str.strip() + "\n"],
            "outputs": [],
            "execution_count": None
        }
        section_list.append(cell)

    def save_notebook(self, filepath):
        final_cells = (
            self.data_exploration_cells +
            self.preprocessing_cells +
            self.model_cells +
            self.prediction_cells
        )

        notebook = {
            "cells": final_cells,
            "metadata": {
                "kernelspec": {
                    "display_name": "Python 3",
                    "language": "python",
                    "name": "python3"
                },
                "language_info": {
                    "name": "python",
                    "version": "3.x"
                }
            },
            "nbformat": 4,
            "nbformat_minor": 2
        }

        with open(filepath, "w", encoding="utf-8") as f:
            json.dump(notebook, f, indent=2)

    def reset_all_sections(self):
        """Optional: wipes everything if user starts over completely."""
        self.data_exploration_cells.clear()
        self.preprocessing_cells.clear()
        self.model_cells.clear()
        self.prediction_cells.clear()

    def reset_downstream(self):
        """Optional: clears everything *except* data exploration."""
        self.preprocessing_cells.clear()
        self.model_cells.clear()
        self.prediction_cells.clear()


exporter = NotebookExporter()

prev_filename = [None]
preprocessing_params = {}
def generate_notebook_filename(export_dir):
    exporter.save_notebook(export_dir)

params_filename = ""
## UTILS
def start_py():
    global params_filename
    params_filename = generate_temp_param_filename()
    print ("start")
    
def read_display(filename):
    try:
        if filename != prev_filename[0]:
            exporter.add_data_exploration(filename=filename)
            prev_filename[0] = filename
            exporter.reset_all_sections()

        # Load data from CSV using Pandas
        df = pd.read_csv(filename)
       
        # Display a sample of the data (first 5 rows)
        sample_data = df.head().to_string(index=False)
        # Capture df.info() into a string
        buffer = io.StringIO()
        df.info(buf=buffer)
        info_data = buffer.getvalue()
        
        
        return f"Sample Data:\n{sample_data}", f"Info:\n {info_data}"

    except Exception as e:
        return f"Error processing I the CSV file: {str(e)}", f"Error getting info the CSV file:{str(e)}"  # Return error message

def Set_preprocessing_params():
    global params_filename, preprocessing_params
    metadata = {}
    if not params_filename:
        raise ValueError("params_filename is not set!")

    if preprocessing_params:  # Only save if not empty
        metadata["preprocessing_params"] = preprocessing_params
    else:
        print("No preprocessing params to save.")
        return
    # Save metadata as JSON (same folder, same base name, .meta.json extension)
    meta_filename = os.path.splitext(params_filename)[0] + ".meta.json"
    with open(meta_filename, 'w') as f:
        json.dump(metadata, f, indent=4)
    print(f"Preprocessing params saved to {meta_filename}")
    return meta_filename

    

def return_columns(filename):
    try:
        df = pd.read_csv(filename)
        # Get columns
        columns = df.columns.tolist()  # Convert to list       
        # Get numeric columns
        numeric_cols = df.select_dtypes(include=["number"]).columns.tolist()

        # Get non-numeric columns
        non_numeric_cols = df.select_dtypes(exclude=["number"]).columns.tolist()

        return columns, numeric_cols, non_numeric_cols

    except Exception as e:
        return [], [], [f"Error processing columns: {str(e)}"]


def split_test_and_train(features, target, test_split, rand_seed):
    try:
        # Split the data into training and testing sets
        X_train, X_test, y_train, y_test = train_test_split(
            features, target, test_size=test_split, random_state=rand_seed
        )
        
        return X_train, X_test, y_train, y_test

    except Exception as e:
        return f"Error splitting test and train data: {str(e)}"

def export_model(model, filename, features=None, target=None, target_classes=None, task_type="classification"):
    # Save the model file (.pkl)
    joblib.dump(model, filename)

    # Prepare metadata dictionary
    metadata = {
        "features": features if features else [],
        "target": target if target else "",
        "task_type": task_type,
    }

    if target_classes:
        metadata["target_classes"] = target_classes
    
    if preprocessing_params:  # <<=== Just check if not empty!
        metadata["preprocessing_params"] = preprocessing_params

    # Save metadata as JSON (same folder, same base name, .meta.json extension)
    meta_filename = os.path.splitext(filename)[0] + ".meta.json"
    with open(meta_filename, 'w') as f:
        json.dump(metadata, f, indent=4)

    print(f"Model saved to {filename}")
    print(f"Metadata saved to {meta_filename}")

def generate_temp_model_filename(model_type):
    
    os.makedirs("temp_models", exist_ok=True)  # Auto-create folder if missing
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"temp_models/{model_type}_{timestamp}_{uuid.uuid4().hex[:6]}.joblib"

def generate_temp_param_filename():
    
    os.makedirs("temp_params", exist_ok=True)  # Auto-create folder if missing
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"temp_params/preprocessing_Params_{timestamp}_{uuid.uuid4().hex[:6]}.json"

def generate_temp_notebook_filename(notebook_name):
    
    os.makedirs("temp_Notebooks", exist_ok=True)  # Auto-create folder if missing
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"temp_models/{notebook_name}_{timestamp}_{uuid.uuid4().hex[:6]}.ipynb"


## PREPROCESSING TECHNICS     
# Apply Expression
def apply_custom_expression(filename, expression, new_column_name):
    try:
        # Load your CSV
        df = pd.read_csv(filename)

        # Evaluate the expression safely in context
        result = eval(expression, {"df": df, "pd": pd})

        # Assign the result to the new column
        df[new_column_name] = result

        # Save it back
        df.to_csv(filename, index=False)

        # Log the *same logic* to the notebook as code
        code_str = f"df['{new_column_name}'] = {expression.strip()}"
        exporter.add_preprocessing(code_str)


        # Return the updated list of columns
        return df.columns.tolist()
    
    except Exception as e:
        return f"Error evaluating expression: {str(e)}"

#Normalization
def apply_normalization(filename, columns, methods, new_column_names):
    try:
        # Load the data
        df = pd.read_csv(filename)

        # Define normalization methods
        normalization_methods = {
            "Min-Max Scaling": MinMaxScaler,
            "Z-score Normalization": StandardScaler,
            "Robust Scaling": RobustScaler
        }

        # Apply the selected normalization methods to each column
        for col, method, new_col in zip(columns, methods, new_column_names):
            if col in df.columns:
                # Ensure column does not contain strings
                if df[col].dtype == object:
                    return f"Error: Column '{col}' contains strings and cannot be normalized."

                if method in normalization_methods:
                    # Create the scaler instance
                    ScalerClass = normalization_methods[method]
                    scaler = ScalerClass()
            
                    df[new_col] = scaler.fit_transform(df[[col]])
                     # === Store fitted params ===
                    if method == "Min-Max Scaling":
                        params = {
                            "min": float(scaler.data_min_[0]),
                            "max": float(scaler.data_max_[0])
                        }
                    elif method == "Z-score Normalization":
                        params = {
                            "mean": float(scaler.mean_[0]),
                            "std": float(scaler.scale_[0])
                        }
                    elif method == "Robust Scaling":
                        params = {
                            "center": float(scaler.center_[0]),  # median
                            "scale": float(scaler.scale_[0])     # IQR
                        }
                    else:
                        params = {}

                    preprocessing_params[new_col] = params
                    # Notebook code log
                    scaler_name = ScalerClass.__name__
                     # === LOG to notebook ===
                    code_str = (
                        f"from sklearn.preprocessing import {scaler_name}\n\n"
                        f"{scaler_name.lower()} = {scaler_name}()\n"
                        f"df['{new_col}'] = {scaler_name.lower()}.fit_transform(df[['{col}']])"
                    )
                    exporter.add_preprocessing(code_str)


        # Save the modified dataframe back to the CSV
        df.to_csv(filename, index=False)

        # Return updated column names (including the new normalized columns)
        return df.columns.tolist()
    except Exception as e:
        return f"Error during normalization: {e}"


def handle_missing_values(filename, columns, method):
    df = pd.read_csv(filename)

    if method == "Drop Rows":
        df.dropna(subset=columns, inplace=True)

        # === Export ===
        code_str = f"df.dropna(subset={columns}, inplace=True)"
        exporter.add_preprocessing(code_str)

    elif method == "Mean":
        for col in columns:
            df[col] = df[col].fillna(df[col].mean())

            code_str = f"df['{col}'] = df['{col}'].fillna(df['{col}'].mean())"
            exporter.add_preprocessing(code_str)

    elif method == "Median":
        for col in columns:
            df[col] = df[col].fillna(df[col].median())

            code_str = f"df['{col}'] = df['{col}'].fillna(df['{col}'].median())"
            exporter.add_preprocessing(code_str)

    elif method == "Mode":
        for col in columns:
            df[col] = df[col].fillna(df[col].mode()[0])

            code_str = f"df['{col}'] = df['{col}'].fillna(df['{col}'].mode()[0])"
            exporter.add_preprocessing(code_str)

    df.to_csv(filename, index=False)
    return df.columns.tolist()


# Label Encoding
def encode_column(filename, column_name, new_column_name=None, encoding_type="label"):
    try:
        df = pd.read_csv(filename)

        if column_name not in df.columns:
            return f"Column '{column_name}' does not exist in the dataset"

        if pd.api.types.is_numeric_dtype(df[column_name]):
            return f"Error: Column '{column_name}' contains numeric values and cannot be encoded."

        if encoding_type == "label":
            if not new_column_name:
                new_column_name = f"{column_name}_label_encoded"
            unique_values = df[column_name].unique()
            encoding_map = {value: idx for idx, value in enumerate(unique_values)}
            df[new_column_name] = df[column_name].map(encoding_map)
            
            # === Export ===
            code_str = (
                f"unique_values = df['{column_name}'].unique()\n"
                f"encoding_map = {{value: idx for idx, value in enumerate(unique_values)}}\n"
                f"df['{new_column_name}'] = df['{column_name}'].map(encoding_map)"
            )
            exporter.add_preprocessing(code_str)

            result = {
                "columns": df.columns.tolist(),
                "encoding_map": encoding_map
            }

        elif encoding_type == "onehot":
            encoded = pd.get_dummies(df[column_name], prefix=column_name).astype(int)
            df = pd.concat([df, encoded], axis=1)

            code_str = (
                f"encoded = pd.get_dummies(df['{column_name}'], prefix='{column_name}').astype(int)\n"
                f"df = pd.concat([df, encoded], axis=1)"
            )
            exporter.add_preprocessing(code_str)

            result = {
                "columns": df.columns.tolist(),
                "encoding_columns": encoded.columns.tolist()
            }

        else:
            return f"Unsupported encoding type: {encoding_type}"

        df.to_csv(filename, index=False)
        return result

    except Exception as e:
        return f"Error during {encoding_type} encoding: {str(e)}"


def apply_standardization(filename, columns, new_column_names):
    try:
        df = pd.read_csv(filename)

        for col, new_col in zip(columns, new_column_names):
            if col in df.columns:
                if df[col].dtype == object:
                    return f"Error: Column '{col}' contains strings and cannot be standardized."

                scaler = StandardScaler()
                df[new_col] = scaler.fit_transform(df[[col]])

                # === Export ===
                code_str = (
                    "from sklearn.preprocessing import StandardScaler\n\n"
                    f"scaler = StandardScaler()\n"
                    f"df['{new_col}'] = scaler.fit_transform(df[['{col}']])"
                )
                exporter.add_preprocessing(code_str)

        df.to_csv(filename, index=False)
        return df.columns.tolist()

    except Exception as e:
        return f"Error during standardization: {e}"

def create_new_column(filename, columns, operation, new_column_name):
    try:
        # Load data
        df = pd.read_csv(filename)

        # Define supported operations
        operations = {
            'sum': sum,
            'product': lambda vals: vals.prod(),  # Use DataFrame product
            'mean': lambda vals: vals.mean()
        }
        
        # Ensure the selected operation is valid
        if operation not in operations:
            if df[col].dtype == object:
                return f"Error: Column '{col}' contains strings and cannot be used for operation '{operation}'."

        # Apply the operation across the selected columns
        if operation == 'sum':
            df[new_column_name] = df[columns].sum(axis=1)
        elif operation == 'product':
            df[new_column_name] = df[columns].prod(axis=1)
        elif operation == 'mean':
            df[new_column_name] = df[columns].mean(axis=1)

        # Save the modified dataframe back to the CSV or return the column names
        df.to_csv(filename, index=False)
        
        # Return updated columns to confirm new column creation
        return df.columns.tolist()

    except Exception as e:
        return f"Error creating new column: {str(e)}"



def remove_columns(filename, columns_to_remove):
    try:
        # Load the data
        df = pd.read_csv(filename)

        # Check if all columns exist in the dataframe
        for col in columns_to_remove:
            if col not in df.columns:
                return f"Column '{col}' not found in the dataset."

        # Drop the specified columns
        df = df.drop(columns=columns_to_remove)

         # === Export ===
        code_str = f"df.drop(columns={columns_to_remove}, inplace=True)"
        exporter.add_preprocessing(code_str)


        # Save the updated dataframe back to the CSV
        df.to_csv(filename, index=False)

        # Return the updated list of columns
        return df.columns.tolist()

    except Exception as e:
        return f"Error removing columns: {str(e)}"




sys.stdout.reconfigure(line_buffering=True)
import matplotlib
matplotlib.use('Agg')  # Use a headless backend that doesn't need GUI
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.model_selection import train_test_split
from sklearn.svm import SVC, SVR
from sklearn.neighbors import KNeighborsClassifier,  KNeighborsRegressor
from sklearn.linear_model import LinearRegression, LogisticRegression, Ridge
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.cluster import KMeans
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, mean_squared_error, r2_score, silhouette_score,  confusion_matrix
from datetime import datetime

print ("Model Imports") 
## Ploting
def generate_plot(csv_path, x_column, y_column, plot_type, output_path="temp_plot.png"):
    try:
        df = pd.read_csv(csv_path)

        plt.figure(figsize=(8, 6))
        color_x = "#3498db"  # Blue for X
        color_y = "#e74c3c"  # Red for Y

        code_lines = [
            "plt.figure(figsize=(8, 6))"
        ]

        if plot_type == "scatter":
            plt.scatter(df[x_column], df[y_column], c=color_y)
            code_lines.append(f"plt.scatter(df['{x_column}'], df['{y_column}'], c='red')")
        elif plot_type == "line":
            plt.plot(df[x_column], color=color_x, label=x_column)
            plt.plot(df[y_column], color=color_y, label=y_column)
            plt.legend()
            code_lines.append(f"plt.plot(df['{x_column}'], label='{x_column}')")
            code_lines.append(f"plt.plot(df['{y_column}'], label='{y_column}')")
            code_lines.append("plt.legend()")
        elif plot_type == "bar":
            code_lines.append("# Bar plot implementation here")
            plt.bar(df.index - 0.2, df[x_column], width=0.4, color=color_x, label=x_column)
            plt.bar(df.index + 0.2, df[y_column], width=0.4, color=color_y, label=y_column)
            plt.legend()
            code_lines.append(f"plt.bar(df.index - 0.2, df['{x_column}'], width=0.4, label='{x_column}')")
            code_lines.append(f"plt.bar(df.index + 0.2, df['{y_column}'], width=0.4, label='{y_column}')")
            code_lines.append("plt.legend()")
        elif plot_type == "hist":
            plt.hist(df[x_column], color=color_x, alpha=0.5, label=x_column)
            plt.hist(df[y_column], color=color_y, alpha=0.5, label=y_column)
            plt.legend()
            code_lines.append(f"plt.hist(df['{x_column}'], alpha=0.5, label='{x_column}')")
            code_lines.append(f"plt.hist(df['{y_column}'], alpha=0.5, label='{y_column}')")
            code_lines.append("plt.legend()")
        else:
            return f"Unsupported plot type: {plot_type}"

        plt.title(f"{plot_type.title()} Plot: {y_column} vs {x_column}")
        plt.tight_layout()
        plt.show()

        code_lines.append(f"plt.title('{plot_type.title()} Plot: {y_column} vs {x_column}')")
        code_lines.append("plt.tight_layout()")
        code_lines.append("plt.show()")

        # === Export ===
        code_str = "\n".join(code_lines)
        exporter.add_Visualization(code_str)

        os.makedirs("temp_images", exist_ok=True)
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"temp_images/{output_path}_{timestamp}_{uuid.uuid4().hex[:6]}.png"

        plt.title(f"{plot_type.title()} Plot: {y_column} vs {x_column}")
        plt.tight_layout()
        plt.savefig(filename)
        plt.close()

        return filename
    except Exception as e:
        return str(e)

def generate_prediction_plot(y_true, y_pred, plot_type="scatter", output_path="temp_pred_plot.png"):
    try:
        # Wrap lists in a DataFrame
        df = pd.DataFrame({
            "True": y_true,
            "Predicted": y_pred
        })

        plt.figure(figsize=(8, 6))

        if plot_type == "scatter":
            plt.scatter(df["True"], df["Predicted"], c="#e74c3c")
        elif plot_type == "line":
            plt.plot(df["True"], label="True", color="#3498db")
            plt.plot(df["Predicted"], label="Predicted", color="#e74c3c")
            plt.legend()
        elif plot_type == "confusion_matrix":
            cm = confusion_matrix(y_true, y_pred, labels=np.unique(y_true))
            # Create a DataFrame for better visualization
            df_cm = pd.DataFrame(cm, index=np.unique(y_true), columns=np.unique(y_true))
            df_cm.index.name = 'Actual'
            df_cm.columns.name = 'Predicted'
            # Plot the heatmap
            sns.heatmap(df_cm, annot=True, fmt="d", cmap="Blues")
            

        else:
            return f"Unsupported plot type: {plot_type}"

        plt.title(f"{plot_type.title()} Plot: True vs Predicted")
        plt.xlabel("True")
        plt.ylabel("Predicted")

        os.makedirs("temp_images", exist_ok=True)
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"temp_images/{output_path}_{timestamp}_{uuid.uuid4().hex[:6]}.png"

        plt.tight_layout()
        plt.savefig(filename)
        plt.close()

        return filename
    except Exception as e:
        return str(e)

## MODELS

def Thread_Regression_Models(model_type, data_path, features, target, random_seed, test_split, output_path="regression_plot.png"):
    print("Start function") 

    df = pd.read_csv(data_path)

    # Prepare data
    X = df[features].values
    y = df[target].values

    # Split the data
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_split, random_state=random_seed)

     

    # Select model
    model_type = model_type.lower()
    if model_type == "linear regression":
        model = LinearRegression()
    elif model_type == "svr":
        model = SVR()
    elif model_type == "ridge":
        model = Ridge()
    elif model_type == "kneighborsregressor":
        model = KNeighborsRegressor(metric='euclidean')
    else:
        raise ValueError(f"Unsupported regression model: {model_type}")
     
    # === Notebook Cell ===
    code_lines = [
        "from sklearn.model_selection import train_test_split",
    ]

    if model_type == "linear regression":
        code_lines.append("from sklearn.linear_model import LinearRegression")
    elif model_type == "svr":
        code_lines.append("from sklearn.svm import SVR")
    elif model_type == "ridge":
        code_lines.append("from sklearn.linear_model import Ridge")
    elif model_type == "kneighborsregressor":
        code_lines.append("from sklearn.neighbors import KNeighborsRegressor")

    code_lines += [
        "from sklearn.metrics import mean_squared_error, r2_score",
        "",
        f'df = pd.read_csv("{data_path}")',
        f'X = df[{features}].values',
        f'y = df["{target}"].values',
        "",
        f'X_train, X_test, y_train, y_test = train_test_split(X, y, test_size={test_split}, random_state={random_seed})',
        "",
        f'model = {model.__class__.__name__}()',
        "model.fit(X_train, y_train)",
        "",
        "y_pred = model.predict(X_test)",
        "",
        "mse = mean_squared_error(y_test, y_pred)",
        "r2 = r2_score(y_test, y_pred)",
        "",
        'print(f"MSE: {mse:.4f}")',
        'print(f"R2 Score: {r2:.4f}")'
    ]

    code_str = "\n".join(code_lines)
    exporter.add_model(code_str)

    # Train model
    model.fit(X_train, y_train)
    y_pred = model.predict(X_test)

    
    # Metrics
    mse = mean_squared_error(y_test, y_pred)
    r2 = r2_score(y_test, y_pred)
    # print(f"Model: {model}, MSE: {mse:.4f}, R^2: {r2:.4f}")

    filename = generate_temp_model_filename(model_type)
    export_model(
        model,
        filename,
        features=features,
        target=target,
        task_type="regression"
    )
    
    # Convert predictions and actuals to basic types
    y_pred_list = y_pred.tolist()
    y_test_list = y_test.tolist()
    print(y_pred)
    print(y_test)
    # Return only C++-friendly data
    return y_pred_list, y_test_list, [mse, r2], filename

        
## Classification_Models
def ThreadClassification_Models(model_type, data_path, features, target, random_seed, test_split):
    
    df = pd.read_csv(data_path)
    X = df[features].values
    y = df[target].values

    target_classes = df[target].unique().tolist()
    
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_split, random_state=random_seed)
    
    model_type = model_type.lower()
    if model_type == "svm":
        model = SVC(kernel='linear', decision_function_shape='ovr')
    elif model_type == "random forest":
        model = RandomForestClassifier(n_estimators=25)
    elif model_type == "logistic regression":
        model = LogisticRegression(random_state=random_seed)
    elif model_type == "gaussiannb":
        model = GaussianNB()
    elif model_type == "decision tree":
        model = DecisionTreeClassifier(criterion="entropy", random_state=100, max_depth=3, min_samples_leaf=5)
    elif model_type == 'kneighborsclassifier':
        model =KNeighborsClassifier(metric='euclidean') 
    else:
        raise ValueError(f"Unsupported classification model: {model_type}")

     # === Notebook Cell ===
    code_lines = [
        "from sklearn.model_selection import train_test_split",
    ]

    if model_type == "svm":
        code_lines.append("from sklearn.svm import SVC")
    elif model_type == "random forest":
        code_lines.append("from sklearn.ensemble import RandomForestClassifier")
    elif model_type == "logistic regression":
        code_lines.append("from sklearn.linear_model import LogisticRegression")
    elif model_type == "gaussiannb":
        code_lines.append("from sklearn.naive_bayes import GaussianNB")
    elif model_type == "decision tree":
        code_lines.append("from sklearn.tree import DecisionTreeClassifier")
    elif model_type == "kneighborsclassifier":
        code_lines.append("from sklearn.neighbors import KNeighborsClassifier")

    code_lines += [
        "from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score",
        "",
        f'df = pd.read_csv("{data_path}")',
        f'X = df[{features}].values',
        f'y = df["{target}"].values',
        "",
        f'X_train, X_test, y_train, y_test = train_test_split(X, y, test_size={test_split}, random_state={random_seed})',
        "",
        f'model = {model.__class__.__name__}()',
        "model.fit(X_train, y_train)",
        "",
        "y_pred = model.predict(X_test)",
        "",
        "accuracy = accuracy_score(y_test, y_pred)",
        "precision = precision_score(y_test, y_pred, average='weighted')",
        "recall = recall_score(y_test, y_pred, average='weighted')",
        "f1 = f1_score(y_test, y_pred, average='weighted')",
        "",
        'print(f"Accuracy: {accuracy:.4f}")',
        'print(f"Precision: {precision:.4f}")',
        'print(f"Recall: {recall:.4f}")',
        'print(f"F1-Score: {f1:.4f}")'
    ]

    code_str = "\n".join(code_lines)
    exporter.add_model(code_str)


    model.fit(X_train, y_train)
    y_pred = model.predict(X_test)
    accuracy = accuracy_score(y_test, y_pred)
    precision = precision_score(y_test, y_pred, average='weighted')
    recall = recall_score(y_test, y_pred, average='weighted')
    f1 = f1_score(y_test, y_pred, average='weighted')
    print(y_pred)
    print(y_test)
    filename = generate_temp_model_filename(model_type)
    export_model(
        model,
        filename,
        features=features,
        target=target,
        target_classes=target_classes,  
        task_type="classification"
    )

    # Convert predictions and actuals to basic types
    y_pred_list = y_pred.tolist()
    y_test_list = y_test.tolist()

    print(f"Model: {model}")
    print(f"Accuracy: {accuracy:.4f}, Precision: {precision:.4f}, Recall: {recall:.4f}, F1-Score: {f1:.4f}")

    # Return only C++-friendly data
    return y_pred_list, y_test_list, [accuracy, precision, recall, f1], filename


def ThreadUnsupervised_Models(model_type, data_path, features, n_clusters, random_seed):
    df = pd.read_csv(data_path)
    X = df[features].values

    if model_type.lower() == "kmeans":
        model = KMeans(n_clusters=n_clusters, random_state=random_seed, n_init="auto")
        labels = model.fit_predict(X)
        score = silhouette_score(X, labels)
    else:
        raise ValueError(f"Unsupported unsupervised model: {model_type}")
    
    # === Notebook Cell ===
    code_lines = [
        "from sklearn.cluster import KMeans",
        "from sklearn.metrics import silhouette_score",
        "",
        f'df = pd.read_csv("{data_path}")',
        f'X = df[{features}].values',
        "",
        f'model = KMeans(n_clusters={n_clusters}, random_state={random_seed}, n_init="auto")',
        "labels = model.fit_predict(X)",
        "",
        "score = silhouette_score(X, labels)",
        "",
        'print(f"Cluster Labels: {labels}")',
        'print(f"Silhouette Score: {score:.4f}")'
    ]

    code_str = "\n".join(code_lines)
    exporter.add_model(code_str)

    filename = generate_temp_model_filename(model_type)
    export_model(
        model,
        filename,
        features=features,
        target=None,
        target_classes=None,
        task_type="unsupervised"
    )

    return labels.tolist(), score, filename


### Model Prediction
def predict_model(filename, features):
    model = joblib.load(filename)

    meta_filename = os.path.splitext(filename)[0] + ".meta.json"
    with open(meta_filename, "r", encoding="utf-8") as file:
        content = json.loads(file.read())

    if len(features) != len(content['features']):
        raise ValueError("Incorrect Numbers of Features!")

    processed_features = []

    for idx, col_name in enumerate(content['features']):
        val = features[idx]
        print(col_name)
        if "_" in col_name:
            base, suffix = col_name.rsplit("_", 1)
            suffix = suffix.lower()

            if suffix == "z-score normalization":
                params = content.get('preprocessing_params', {}).get(col_name)
                print(params)
                if not params:
                    raise ValueError(f"No scaler params for {col_name}")

                mean = params['mean']
                scale = params['scale']
                val = (val - mean) / scale
                print(val)
            elif suffix == "min-max scaling":
                print("\n=== DEBUG ===")
                print("col_name:", col_name)
                print("Available keys:", list(content.get('preprocessing_params', {}).keys()))
                print("================\n")
                print(col_name)
                params = content.get('preprocessing_params', {}).get(col_name)
                print(params)
                if not params:
                    raise ValueError(f"No scaler params for {col_name}")
                
                min_ = params['min']
                max_ = params['max']
                val = (val - min_) / (max_ - min_)
                print(val)
            elif suffix == "robust scaling":
                params = content.get('preprocessing_params', {}).get(col_name)
                print(params)
                if not params:
                    raise ValueError(f"No scaler params for {col_name}")

                median = params['median']
                iqr = params['iqr']
                val = (val - median) / iqr
                print(val)

            # Add more techniques as needed.

        # else: no preprocessing needed
        processed_features.append(val)

    # === Notebook Cell ===
    code_lines = [
        "import joblib",
        "import json",
        "",
        f'model = joblib.load(\"{filename}\")',
        f'with open(\"{meta_filename}\", \"r\", encoding=\"utf-8\") as file:',
        "    content = json.load(file)",
        "",
        f"# Raw input features: {features}",
        f"processed_features = {processed_features}",
        "",
        "prediction = model.predict([processed_features])",
        "",
        'print(\"Prediction:\", prediction[0])'
    ]

    code_str = "\n".join(code_lines)
    exporter.add_prediction(code_str)

    prediction = model.predict([processed_features])
    return prediction[0]