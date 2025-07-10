import json

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
            self.data_exploration_cells
            + self.preprocessing_cells
            + self.model_cells
            + self.prediction_cells
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

    def reset_sections(self):
        self.preprocessing_cells.clear()
        self.model_cells.clear()
        self.prediction_cells.clear()
