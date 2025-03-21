import asyncio
import nominatim_api as napi

# python3 -m pip install nominatim-api
# python3 -m pip install packaging/nominatim-api

async def search(query):
    async with napi.NominatimAPIAsync() as api:
        return await api.search(query)

results = asyncio.run(search('Brugge'))
if not results:
    print('Cannot find Brugge')
else:
    print(f'Found a place at {results[0].centroid.x},{results[0].centroid.y}')