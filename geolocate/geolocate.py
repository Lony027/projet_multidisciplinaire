import pandas as pd
from dotenv import load_dotenv
from InquirerPy import inquirer
import os
import openrouteservice
import numpy as np
from tqdm import trange, tqdm

def choose_csv():
    folder = "data"
    csv_files = [f for f in os.listdir(folder) if f.endswith(".csv")]

    if not csv_files:
        raise FileNotFoundError("No CSV files found in the 'data' directory.")

    selected_file = inquirer.select(
        message="Choose a CSV file:",
        choices=csv_files,
    ).execute()

    filename = os.path.join(folder, selected_file)
    print(f"Selected file: {filename}")
    return filename

order_filename = choose_csv()

load_dotenv()
ORS_API_KEY = os.getenv("ORS_API_KEY")

column_names = ["name", "street", "postalcode", "city"] 
df = pd.read_csv(order_filename, header=None, names=column_names)

# REVERSE GEOCODE

client = openrouteservice.Client(key=ORS_API_KEY)
df['full_address'] = df['street'] + ', ' + df['postalcode'].astype(str) + ' ' + df['city'] + ' France'

def get_coordinates(address):
    try:
        geocode = client.pelias_search(text=address)
        coords = geocode['features'][0]['geometry']['coordinates']
        return coords[1], coords[0]  # lat, lon
    except Exception as e:
        print(f"Error geocoding {address}: {e}")
        return None, None


tqdm.pandas()
df[['latitude', 'longitude']] = df['full_address'].progress_apply(lambda x: pd.Series(get_coordinates(x)))
df = df.drop(columns=['full_address'])
geocoded_filename = "output/geocoded_output.csv"
df.to_csv(geocoded_filename, index=False)

print("Reverse geocoded output saved to 'geocoded_output.csv'")

# MATRIX GENERATIONS

df = pd.read_csv(geocoded_filename, usecols=['latitude', 'longitude'])
coords = df[['longitude', 'latitude']].values.tolist() 

n = len(coords)
batch_size = 50
distance_matrix = np.zeros((n, n))
duration_matrix = np.zeros((n, n))

for i in trange(0, n, batch_size):
    for j in range(0, n, batch_size):
        sources = coords[i:i+batch_size]
        destinations = coords[j:j+batch_size]

        try:
            response = client.distance_matrix(
                locations=sources + destinations,
                sources=list(range(len(sources))),
                destinations=list(range(len(sources), len(sources) + len(destinations))),
                metrics=["distance", "duration"],
                units="m"
            )

            distances = np.array(response['distances'])
            durations = np.array(response['durations'])

            distance_matrix[i:i+len(sources), j:j+len(destinations)] = distances
            duration_matrix[i:i+len(sources), j:j+len(destinations)] = durations

        except openrouteservice.exceptions.ApiError as e:
            print(f"API error for block ({i}, {j}): {e}")
        except Exception as e:
            print(f"Unexpected error for block ({i}, {j}): {e}")

pd.DataFrame(distance_matrix).to_csv("output/distance_matrix.csv", index=False, header=False)
pd.DataFrame(duration_matrix).to_csv("output/duration_matrix.csv", index=False, header=False)

print("Matrices saved to 'distance_matrix.csv' and 'duration_matrix.csv'")