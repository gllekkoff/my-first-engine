import pandas as pd
import matplotlib.pyplot as plt

def plot_performance_data():
    """
    Reads performance data from CSV files and plots a comparison graph.
    """
    try:
        data_non_instanced = pd.read_csv("dataPlots/performance_non_instanced1k.csv")
        data_instanced = pd.read_csv("dataPlots/performance_instanced1k.csv")

        plt.figure(figsize=(12, 7))

        plt.plot(data_non_instanced["Time (s)"], data_non_instanced["FrameTime (ms)"],
                 label="Without Instancing", color='red', marker='o', linestyle='--')

        plt.plot(data_instanced["Time (s)"], data_instanced["FrameTime (ms)"],
                 label="With Instancing", color='green', marker='x', linestyle='-')

        plt.axhline(y=16.67, color='blue', linestyle=':', label='60 FPS Target (16.67 ms)')

        plt.title("Performance Comparison: Instanced vs. Non-Instanced Rendering")
        plt.xlabel("Time (seconds)")
        plt.ylabel("Frame Time (milliseconds)")
        plt.legend()
        plt.grid(True)

        plt.show()

    except FileNotFoundError as e:
        print(f"Error: Could not find a data file. Make sure both CSV files exist.")
        print(f"Missing file: {e.filename}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    plot_performance_data()
