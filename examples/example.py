import time
import csv
import os
from quantengine import Engine, Tick

def main():
    """
    Example of how to use the quantengine with CSV data.
    """
    print("Initializing the quant engine...")
    # These values should be powers of two
    ring_buffer_size = 1024
    window_size = 4096 
    
    engine = Engine(ringBufferSize=ring_buffer_size, windowSize=window_size)
    
    print("Starting the engine...")
    engine.start()
    
    # Get the directory of the example script
    script_dir = os.path.dirname(os.path.realpath(__file__))
    # Construct the path to the CSV file
    csv_path = os.path.join(script_dir, '..', 'data', 'ticks.csv')

    print(f"Ingesting data from {csv_path}...")
    with open(csv_path, 'r') as f:
        reader = csv.reader(f)
        header = next(reader) # Skip header
        for row in reader:
            tick = Tick()
            tick.ts_ns = int(row[0])
            tick.price = float(row[1])
            tick.size = float(row[2])
            tick.symbol_id = int(row[3])
            engine.ingest_tick(tick)

    print("Waiting for stats to compute...")
    time.sleep(1) # Give consumer thread time to process

    stats = engine.snapshot()
    if stats.count > 0:
        print(f"Tick count: {stats.count}, Mean: {stats.mean:.2f}, Std: {stats.std:.2f}, Vwap: {stats.vwap:.2f}")
    else:
        print("No stats available.")
            
    print("Stopping the engine...")
    engine.stop()
    print("Done.")

if __name__ == "__main__":
    main()