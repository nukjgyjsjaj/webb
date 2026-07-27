# GeoIP Database Setup

## Required Files
1. GeoLite2-City.mmdb - MaxMind GeoLite2 City database
2. GeoLite2-ASN.mmdb - MaxMind GeoLite2 ASN database

## Download Instructions

### Option 1: MaxMind GeoLite2 (Free)
1. Create account at https://www.maxmind.com/en/geolite2/signup
2. Download GeoLite2-City.tar.gz and GeoLite2-ASN.tar.gz
3. Extract and place in this directory:
   - GeoLite2-City.mmdb
   - GeoLite2-ASN.mmdb

### Option 2: Use Local Copy
- Place downloaded .mmdb files in this directory
- Update paths in build_configs/*/args.gn if needed

### Option 3: Build at Runtime
The application can download and update the database automatically using the MaxMind GeoIP Update API.

## Usage
```cpp
GeoInfo geo = GeoIPDatabase::Lookup(proxy_ip);
// Returns: country, region, city, latitude, longitude, timezone, ASN
```

## Privacy Note
The GeoIP database is used to derive the spoofed location from the proxy IP. The user's real IP is never queried against this database.
