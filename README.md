# DOOR OPENING SYSTEM 
 Web for manager device lock door with php (deploy by xampp, phpmyadmin)

## Function

- Show state of door (lock or unlock)
- Remote lock/unlock door
- Notification when someone input password wrong more than three times
- Logs activity of devices
- Manager account of user

## Set up

- Install xampp, put this project into folder httpdocs of xampp and browser: http://127.0.0.1/Web_LockDoor/sources/login.php
- Database:
    - Go to http://127.0.0.1/phpmyadmin/index.php
    - Login as admin (admin:admin123)
    - Add 1 user account: nbp1:passMySQL
    - Add database fruit_shop
    - Paste SQL query in file database/fruit_shop.sql to SQL in db fruit_shop
