# How to Use This Program

## 1. Create an Environment File

At the root of the project, create a .env file containing your OpenRouteService API key.

It should look like this:
```
ORS_API_KEY=your_api_key
```
You can obtain a free API key by signing up at openrouteservice.org.

## 2. Input Your Deliveries

Place your delivery data as a .csv file inside the delivery folder.
The CSV must *not contain a header row* and follow this format:
```
"name", "street", "postal_code", "city"
```

## 3. Launch the Script

To run the program, execute the following command from the root of the project:

```bash
./launch.sh
```

If you want to enable route visualization for debugging purposes (used in the genetic algorithm), you can pass the -G flag as an argument:

```bash
./launch.sh -G
```

Make sure the script has executable permissions. If needed, run:

```bash
chmod +x launch.sh
```