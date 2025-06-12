import os
import pandas as pd
import numpy as np
from tqdm import trange, tqdm
from dotenv import load_dotenv
from InquirerPy import inquirer
import openrouteservice

# Emplacement du script actuel
script_dir = os.path.dirname(os.path.abspath(__file__))
print("script_dir:", script_dir)

def choose_csv():
    # On revient dans le dossier parent de script_dir
    folder = os.path.abspath(os.path.join(script_dir, "..","..", "delivery"))
    print("folder:", folder)

    csv_files = [f for f in os.listdir(folder) if f.endswith(".csv")]

    if not csv_files:
        raise FileNotFoundError("No CSV files found in the parent directory.")

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

# pass as a parameter !!!
cerp_address = [["Entrepot CERP", "600 Rue des Madeleines", "77100", "Mareuil-lès-Meaux"]]

new_row = pd.DataFrame(cerp_address, columns=column_names)
df = pd.concat([new_row, df], ignore_index=True)

def modify_address(address):
    if "ZAC vaux, 4 Pl. de l'Île de France" in address:
        return address.replace("ZAC vaux, 4 Pl. de l'Île de France", "4 Pl. de l'Île de France")

    if "24 Av. de la Font du Berger" in address:
        return address.replace("24 Av. de la Font du Berger", "24 Av. de la Fontaine du Berger")

    return address

df['full_address'] = df['street'] + ', ' + df['postalcode'].astype(str) + ' ' + df['city'] + ' France'

# REVERSE GEOCODE

# Loading Cache
cache_file = os.path.join(script_dir,"cache","cache_geocoding.csv");
if os.path.exists(cache_file):
    cache_df = pd.read_csv(cache_file)
else:
    cache_df = pd.DataFrame(columns=["full_address", "latitude", "longitude"])

cache_dict = dict(zip(cache_df['full_address'], zip(cache_df['latitude'], cache_df['longitude'])))


client = openrouteservice.Client(key=ORS_API_KEY)

def get_coordinates(address):
    if address in cache_dict:
        return cache_dict[address]

    try:
        geocode = client.pelias_search(text=modify_address(address))
        coords = geocode['features'][0]['geometry']['coordinates']
        lat, lon = coords[1], coords[0]
        cache_dict[address] = (lat, lon)
        return lat, lon
    except Exception as e:
        print(f"Error geocoding {address}: {e}")
        return None, None


tqdm.pandas()
df[['latitude', 'longitude']] = df['full_address'].progress_apply(
    lambda x: pd.Series(get_coordinates(x))
)
df = df.drop(columns=['full_address'])
# all file path should be at the beginning!!


output_dir = os.path.join(script_dir, "output")
os.makedirs(output_dir, exist_ok=True)  # Crée le dossier 'output' s'il n'existe pas
geocoded_filename = os.path.join(output_dir,"geocoded_output.csv")

df.to_csv(geocoded_filename, index=False)

print("Reverse geocoded output saved to 'geocoded_output.csv'")

updated_cache = pd.DataFrame([
    {'full_address': addr, 'latitude': lat, 'longitude': lon}
    for addr, (lat, lon) in cache_dict.items() if lat is not None and lon is not None
])
updated_cache.to_csv(cache_file, index=False)

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

distance_matrix_path = os.path.join(output_dir,"distance_matrix.csv")
duration_matrix_path = os.path.join(output_dir,"duration_matrix.csv")
pd.DataFrame(distance_matrix).to_csv(distance_matrix_path, index=False, header=False)
pd.DataFrame(duration_matrix).to_csv(duration_matrix_path, index=False, header=False)

print("Matrices saved to 'distance_matrix.csv' and 'duration_matrix.csv'")

