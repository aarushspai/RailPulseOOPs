# RailPulse — Smart Train Companion

Student project prototype based on the uploaded C Train Reservation System.

## Frontend
Open `index.html` in a browser. It includes a railway dashboard, sample train tracking, schematic route explorer, browser-only demo booking, and PNR lookup.

## C code
`railpulse.c` is your reservation program extended with route stations and manual/demo location checking.

Compile with GCC:
```bash
gcc railpulse.c -o railpulse
./railpulse
```
On Windows with MinGW:
```bash
gcc railpulse.c -o railpulse.exe
railpulse.exe
```

## Important
This project does not provide real-time train tracking. The frontend uses illustrative sample data, and the C program's station location is manually updated. Real live status requires a trusted/authorized data source and a backend integration. The frontend and C program are separate for now; they are not connected to each other.

## GitHub Pages
Upload `index.html`, `style.css`, and `app.js` to the repository root. The included workflow can deploy the static frontend through GitHub Pages. C source files do not execute in the browser.
