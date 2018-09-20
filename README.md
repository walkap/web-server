# web-server
Internet and Web project

Per compilare l’applicazione da linea di comando eseguire i seguenti passaggi:
- Assicurarsi che nella root del progetto sia presente il file CMakeLists.txt
- Dalla root del progetto eseguire il comando “mkdir build” per creare una cartella dove andranno a finire i file binari (il nome non è importante abbiamo scelto build per semplicità di comprensione)
- Una volta creata la cartella eseguire il comando “cd build” per posizionarsi all’interno della cartella in questione
- A questo punto possiamo eseguire il comando “cmake -G ‘Unix Makefiles’ ..” senza dimenticare i puntini finali che servono per dire a cmake che nella cartella superiore è presente il file di configurazione CMakeLists.txt. 
- Eseguire ora il comando “make main” dove main è il target principale scelto per poter compilare e che ha associate tutte le sue dipendenze
- Se abbiamo eseguito correttamente tutti i passaggi possiamo finalmente avviare il nostro web server eseguendo il comando “./main”
