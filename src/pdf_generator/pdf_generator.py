import os
from datetime import datetime, timedelta

import pandas as pd
import matplotlib.pyplot as plt
from shapely.geometry import LineString, Point
import geopandas as gpd
import contextily as ctx
from PIL import Image
from fpdf import FPDF
from fpdf.enums import XPos, YPos





# --- Constants ---
START_TIME = datetime.strptime("09:00", "%H:%M")
STOP_DURATION = timedelta(minutes=3)
WAREHOUSE_INDEX = 0

FUEL_CONSUMPTION_L_PER_100KM = 8.5
FUEL_PRICE_EUR_PER_L = 1.72

MAX_MAP_WIDTH_MM = 120
PDF_WIDTH_MM = 210  # A4 size in mm

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

FONT_PATH = os.path.join(BASE_DIR, "fonts", "DejaVuSans.ttf")
FONT_BOLD_PATH = os.path.join(BASE_DIR, "fonts", "DejaVuSans-Bold.ttf")
FONT_NAME = "DejaVu"

# --- Load data ---
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, "..", "..")) 

routes = pd.read_csv(os.path.join(PROJECT_ROOT, "src.csv", "genetic", "output"), header=None)
locations = pd.read_csv(os.path.join(PROJECT_ROOT, "src", "geolocate", "output", "geocoded_output.csv"))
time_matrix = pd.read_csv(os.path.join(PROJECT_ROOT, "src", "geolocate", "output", "duration_matrix.csv"), header=None)
distance_matrix = pd.read_csv(os.path.join(PROJECT_ROOT, "src", "geolocate", "output", "distance_matrix.csv"), header=None)

# --- Helper ---
def format_time(t: datetime) -> str:
    return t.strftime("%Hh%M")

# --- Process each route ---
for i, route in routes.iterrows():
    route_indices = route.dropna().astype(int).tolist()
    lats = [locations.iloc[idx]['latitude'] for idx in route_indices]
    longs = [locations.iloc[idx]['longitude'] for idx in route_indices]

    # Initialize PDF
    pdf = FPDF()
    pdf.set_auto_page_break(auto=True, margin=15)
    pdf.add_page()
    pdf.add_font(FONT_NAME, "", FONT_PATH)
    pdf.add_font(FONT_NAME, "B", FONT_BOLD_PATH)
    pdf.set_font(FONT_NAME, 'B', 14)
    pdf.cell(0, 10, f"Parcours camionnette {i+1}", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

    # Generate Map
    points = [Point(lon, lat) for lon, lat in zip(longs, lats)]
    gdf = gpd.GeoDataFrame(geometry=points, crs="EPSG:4326").to_crs(epsg=3857)
    line = gpd.GeoDataFrame(geometry=[LineString(gdf.geometry)], crs=gdf.crs)

    fig, ax = plt.subplots(figsize=(8, 8))
    gdf.plot(ax=ax, marker='o', color='#888888', markersize=40)
    gdf.iloc[[0]].plot(ax=ax, marker='o', color='#4CAF50', markersize=60)
    line.plot(ax=ax, linewidth=2.5, color='#607D8B')

    x_min, y_min, x_max, y_max = gdf.total_bounds
    max_range = max(x_max - x_min, y_max - y_min) * 1.2
    x_center, y_center = (x_min + x_max) / 2, (y_min + y_max) / 2
    half_range = max_range / 2
    ax.set_xlim(x_center - half_range, x_center + half_range)
    ax.set_ylim(y_center - half_range, y_center + half_range)

    ctx.add_basemap(ax, source=ctx.providers.OpenStreetMap.Mapnik)
    ax.axis("off")
    if ax.get_legend():
        ax.get_legend().remove()

    map_filename = f"map_camionnette_{i+1}.png"
    plt.savefig(map_filename, bbox_inches="tight", dpi=150)
    plt.close()

    # Add map to PDF
    with Image.open(map_filename) as img:
        width_px, height_px = img.size
        dpi = img.info.get('dpi', (96,))[0]
        width_mm = width_px / dpi * 25.4
        height_mm = height_px / dpi * 25.4

    if width_mm > MAX_MAP_WIDTH_MM:
        scale = MAX_MAP_WIDTH_MM / width_mm
        width_mm *= scale
        height_mm *= scale

    x_offset = (PDF_WIDTH_MM - width_mm) / 2
    pdf.image(map_filename, x=x_offset, w=width_mm, h=height_mm)
    os.remove(map_filename)

    # Table Headers
    pdf.set_font(FONT_NAME, 'B', 8)
    pdf.cell(50, 10, "Étape", 1)
    pdf.cell(90, 10, "Adresse", 1)
    pdf.cell(25, 10, "Arrivé", 1)
    pdf.cell(25, 10, "Départ", 1)
    pdf.ln()

    # Route Details
    pdf.set_font(FONT_NAME, '', 8)
    current_time = START_TIME
    total_distance_m = 0
    prev_index = route_indices[0]

    for j, idx in enumerate(route_indices):
        is_first = j == 0
        is_last = j == len(route_indices) - 1

        arrival_time = None
        if not is_first:
            travel_seconds = time_matrix.iat[prev_index, idx]
            travel_distance = distance_matrix.iat[prev_index, idx]
            current_time += timedelta(seconds=travel_seconds)
            total_distance_m += travel_distance
            arrival_time = current_time

        departure_time = None
        if not is_last:
            departure_time = current_time + STOP_DURATION if not is_first else current_time

        row = locations.iloc[idx]
        address = f"{row['street']} {row['postalcode']} {row['city']}"
        name = (
            "Départ " + row['name'] if is_first else
            "Retour " + row['name'] if is_last and idx == WAREHOUSE_INDEX else
            row['name']
        )

        y_start = pdf.get_y()
        pdf.multi_cell(50, 10, name, border=1)
        y_mid = pdf.get_y()
        pdf.set_y(y_start)
        pdf.set_x(60)
        pdf.multi_cell(90, 10, address, border=1)
        y_end = max(pdf.get_y(), y_mid)
        pdf.set_y(y_start)
        pdf.set_x(150)
        pdf.cell(25, 10, format_time(arrival_time) if arrival_time else "", border=1)
        pdf.cell(25, 10, format_time(departure_time) if departure_time else "", border=1)
        pdf.ln(y_end - y_start)

        if not is_last:
            current_time = departure_time
            prev_index = idx

    # Summary
    pdf.ln(5)
    pdf.set_font(FONT_NAME, 'B', 10)
    total_km = total_distance_m / 1000
    fuel_l = (total_km * FUEL_CONSUMPTION_L_PER_100KM) / 100
    fuel_cost = fuel_l * FUEL_PRICE_EUR_PER_L

    pdf.cell(0, 10, f"Distance totale parcourue : {total_km:.1f} km", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
    pdf.cell(0, 10, f"Carburant estimé : {fuel_l:.2f} L (8.5 L / 100km)", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
    pdf.cell(0, 10, f"Coût carburant estimé : {fuel_cost:.2f} € (1.72 €/L)", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

    # Save PDF
    pdf.output(f"parcours_camionnette_{i+1}.pdf")

print("PDFs générés pour chaque camionnette.")