import os
from datetime import datetime, timedelta

import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from shapely.geometry import LineString, Point
import geopandas as gpd
import contextily as ctx
from PIL import Image
from fpdf import FPDF
from fpdf.enums import XPos, YPos

# CONSTANTS
START_TIME = datetime.strptime("09:00", "%H:%M")
STOP_DURATION = timedelta(minutes=3)

FUEL_CONSUMPTION_L_PER_100KM = 8.5
FUEL_PRICE_EUR_PER_L = 1.72

MAX_MAP_WIDTH_MM = 120
PDF_WIDTH_MM = 210

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, "..", "..")) 

FONT_PATH = os.path.join(BASE_DIR, "fonts", "DejaVuSans.ttf")
FONT_BOLD_PATH = os.path.join(BASE_DIR, "fonts", "DejaVuSans-Bold.ttf")
FONT_NAME = "DejaVu"

COLORS = [
    "#e6194b", "#3cb44b", "#ffe119", "#4363d8", "#f58231", "#911eb4",
    "#46f0f0", "#f032e6", "#bcf60c", "#fabebe", "#008080", "#e6beff",
    "#9a6324", "#fffac8", "#800000", "#aaffc3", "#808000", "#ffd8b1"
]

PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, "..", "..")) 

#LOAD DATA

with open(os.path.join(PROJECT_ROOT, "src", "genetic", "output.csv")) as f:
    routes = [line.strip().split(",") for line in f if line.strip()]


locations = pd.read_csv(os.path.join(PROJECT_ROOT, "src", "geolocate", "output", "geocoded_output.csv"))
time_matrix = pd.read_csv(os.path.join(PROJECT_ROOT, "src", "geolocate", "output", "duration_matrix.csv"), header=None)
distance_matrix = pd.read_csv(os.path.join(PROJECT_ROOT, "src", "geolocate", "output", "distance_matrix.csv"), header=None)

global_routes = []  # Pour les lignes
global_summary = []  # Pour les stats
all_points = []  # Pour stocker tous les points de passage

# --- Helper ---
def format_time(t: datetime) -> str:
    return t.strftime("%Hh%M")

# --- Process each route ---
for i, route in enumerate(routes):
    route_indices = list(map(int, route))
    lats = [locations.iloc[idx]['latitude'] for idx in route_indices]
    longs = [locations.iloc[idx]['longitude'] for idx in route_indices]
    
    # Add points to global collection
    for idx in route_indices:
        all_points.append(Point(locations.iloc[idx]['longitude'], locations.iloc[idx]['latitude']))
    
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
    pdf.ln(5)
    pdf.set_font(FONT_NAME, 'B', 8)
    
    # Définir les largeurs des colonnes
    col_width_etape = 50
    col_width_adresse = 90
    col_width_time = 25
    row_height = 8  # Hauteur fixe pour les cellules, augmentée pour plus de consistance
    
    pdf.cell(col_width_etape, row_height, "Étape", 1)
    pdf.cell(col_width_adresse, row_height, "Adresse", 1)
    pdf.cell(col_width_time, row_height, "Arrivé", 1)
    pdf.cell(col_width_time, row_height, "Départ", 1)
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
            "Retour " + row['name'] if is_last and idx == 0 else
            row['name']
        )

        # Calculer combien de lignes nécessaires pour le texte
        name_lines = max(1, len(name) // 20 + (1 if len(name) % 20 > 0 else 0))
        address_lines = max(1, len(address) // 35 + (1 if len(address) % 35 > 0 else 0))
        max_lines = max(name_lines, address_lines)
        
        # Hauteur minimale garantie avec un peu plus d'espace
        cell_height = max(row_height, row_height * max_lines + 2)
        
        # Sauvegarder la position X
        x_start = pdf.get_x()
        y_start = pdf.get_y()
        
        # Vérifier si on a besoin d'une nouvelle page
        if y_start + cell_height > pdf.page_break_trigger:
            pdf.add_page()
            y_start = pdf.get_y()
        
        # Créer une fonction pour dessiner des cellules de hauteur fixe
        def draw_filled_cell(x, y, width, height, txt, border=1):
            pdf.set_xy(x, y)
            # Dessiner un rectangle plein
            pdf.cell(width, height, "", border)
            # Placer le texte au milieu
            pdf.set_xy(x, y + (height - row_height) / 2)
            # Utiliser une hauteur de ligne plus petite pour éviter le débordement
            line_height = min(row_height, height / max_lines)
            pdf.multi_cell(width, line_height, txt, 0)
        
        # Étape
        draw_filled_cell(x_start, y_start, col_width_etape, cell_height, name)
        
        # Adresse
        draw_filled_cell(x_start + col_width_etape, y_start, col_width_adresse, cell_height, address)
        
        # Arrivée
        draw_filled_cell(x_start + col_width_etape + col_width_adresse, y_start, 
                        col_width_time, cell_height, format_time(arrival_time) if arrival_time else "")
        
        # Départ
        draw_filled_cell(x_start + col_width_etape + col_width_adresse + col_width_time, y_start, 
                        col_width_time, cell_height, format_time(departure_time) if departure_time else "")
        
        # Positionner à la ligne suivante
        pdf.set_y(y_start + cell_height)

        if not is_last:
            current_time = departure_time
            prev_index = idx

    # Summary
    total_km = total_distance_m / 1000
    fuel_l = (total_km * FUEL_CONSUMPTION_L_PER_100KM) / 100
    fuel_cost = fuel_l * FUEL_PRICE_EUR_PER_L

    color = COLORS[i % len(COLORS)]

    line_gdf = gpd.GeoDataFrame(geometry=[LineString(gdf.geometry)], crs=gdf.crs)
    global_routes.append({
        "geometry": line_gdf.geometry.iloc[0],
        "color": color,
        "label": f"Camionnette {i+1}"
    })


    global_summary.append({
        "camionnette": f"Camionnette {i+1}",
        "distance_km": total_km,
        "fuel_l": fuel_l,
        "fuel_cost": fuel_cost,
    })

    # Save PDF
    pdf.output(f"parcours_camionnette_{i+1}.pdf")


# Générer la carte globale
fig, ax = plt.subplots(figsize=(10, 10))

legend_elements = []

# Créer un GeoDataFrame avec tous les points
points_gdf = gpd.GeoDataFrame(geometry=all_points, crs="EPSG:4326").to_crs(epsg=3857)

# Définir les limites de la carte
x_min, y_min, x_max, y_max = points_gdf.total_bounds
max_range = max(x_max - x_min, y_max - y_min) * 1.2
x_center, y_center = (x_min + x_max) / 2, (y_min + y_max) / 2
half_range = max_range / 2
ax.set_xlim(x_center - half_range, x_center + half_range)
ax.set_ylim(y_center - half_range, y_center + half_range)

# Ajouter les routes au graphique
for route in global_routes:
    gpd.GeoSeries([route["geometry"]], crs="EPSG:3857").plot(
        ax=ax,
        linewidth=2.5,
        color=route["color"],
        label=route["label"]
    )
    legend_elements.append(Line2D([0], [0], color=route["color"], lw=3, label=route["label"]))

# Ajouter les points
points_gdf.plot(ax=ax, marker='o', color='black', markersize=20)

ctx.add_basemap(ax, source=ctx.providers.OpenStreetMap.Mapnik)
ax.axis("off")
ax.legend(handles=legend_elements, loc='upper left', fontsize='small')

global_map_path = "carte_globale.png"
plt.savefig(global_map_path, bbox_inches="tight", dpi=150)
plt.close()

# Créer le PDF global
pdf_global = FPDF()
pdf_global.set_auto_page_break(auto=True, margin=15)
pdf_global.add_page()
pdf_global.add_font(FONT_NAME, "", FONT_PATH)
pdf_global.add_font(FONT_NAME, "B", FONT_BOLD_PATH)
pdf_global.set_font(FONT_NAME, 'B', 14)
pdf_global.cell(0, 10, "Carte globale des parcours", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

# Ajouter la carte
with Image.open(global_map_path) as img:
    width_px, height_px = img.size
    dpi = img.info.get('dpi', (96,))[0]
    width_mm = width_px / dpi * 25.4
    height_mm = height_px / dpi * 25.4

if width_mm > MAX_MAP_WIDTH_MM:
    scale = MAX_MAP_WIDTH_MM / width_mm
    width_mm *= scale
    height_mm *= scale

x_offset = (PDF_WIDTH_MM - width_mm) / 2
pdf_global.image(global_map_path, x=x_offset, w=width_mm, h=height_mm)
os.remove(global_map_path)

# Résumé des camionnettes
pdf_global.ln(5)
pdf_global.set_font(FONT_NAME, 'B', 12)
pdf_global.cell(0, 10, "Résumé carburant et distances par camionnette", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

pdf_global.set_font(FONT_NAME, '', 10)

total_distance = 0
total_fuel = 0
total_cost = 0

for item in global_summary:
    total_distance += item["distance_km"]
    total_fuel += item["fuel_l"]
    total_cost += item["fuel_cost"]

# Détails supplémentaires
pdf_global.set_font(FONT_NAME, 'B', 10)
pdf_global.ln(5)
pdf_global.cell(0, 10, f"Distance totale : {total_distance:.0f} km", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
pdf_global.cell(0, 10, f"Consommation estimée : {total_fuel:.2f}L (8.5 L / 100km)", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
pdf_global.cell(0, 10, f"Prix du carburant : {fuel_cost:.2f} € (diesel : 1.72 €/L)", new_x=XPos.LMARGIN, new_y=YPos.NEXT)

# Sauver le PDF global
pdf_global.output("parcours_global.pdf")

print("PDFs générés pour chaque camionnette et PDF global.")