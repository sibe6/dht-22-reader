# temperature_webserver
Program that reads temperature from DHT-22 sensor on Raspberry, saves the data to .csv where it is read with PHP-script.

## How to install

### Prequisities
Apache Server running on Raspberry Pi.

### Steps:
Move:
- data.csv  
- data.js  
- data.php  
- index.html
-  style.css
- latest_data.php

to ```/var/www/html```

```bash=
sudo mv data.csv data.js data.php index.html style.css latest_data.php /var/www/html/
```

Run temperature reader
```bash=
sudo ./temperature
```

