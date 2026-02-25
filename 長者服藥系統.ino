#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// BUZZER PIN: 12
#define BUZZER 12
// MUSIC NOTES
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976

// MUSIC MELODIES_1
const int size_littlestar = 42;
const int melody_littlestar[] = {
    NOTE_C5, NOTE_C5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_A5, NOTE_G5, // Twinkle, twinkle, little star,
    NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_C5, // How I wonder what you are!
    NOTE_G5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, // Up above the world so high,
    NOTE_G5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, // Like a diamond in the sky.
    NOTE_C5, NOTE_C5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_A5, NOTE_G5, // Twinkle, twinkle, little star,
    NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_C5  // How I wonder what you are!
};
const int noteDurations_littlestar[] = {
    4, 4, 4, 4, 4, 4, 2,  // Twinkle, twinkle, little star,
    4, 4, 4, 4, 4, 4, 2,  // How I wonder what you are!
    4, 4, 4, 4, 4, 4, 2,  // Up above the world so high,
    4, 4, 4, 4, 4, 4, 2,  // Like a diamond in the sky.
    4, 4, 4, 4, 4, 4, 2,  // Twinkle, twinkle, little star,
    4, 4, 4, 4, 4, 4, 2   // How I wonder what you are!
};
// MUSIC MELODIES_2
const int size_merrychristmas = 30;
const int melody_merrychristmas[] = {
    NOTE_D5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_FS5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_A5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_FS5, NOTE_D5,
    NOTE_D5, NOTE_B5, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_E5,
    NOTE_D5, NOTE_D5, NOTE_E5, NOTE_A5, NOTE_FS5, NOTE_G5
};
const int noteDurations_merrychristmas[] = {
    4, 4, 8, 8, 8, 8, 4, 4,
    4, 4, 8, 8, 8, 8, 4, 4,
    4, 4, 8, 8, 8, 8, 4, 4,
    8, 8, 4, 4, 4, 2
};
// MUSIC MELODIES_3
const int size_happybirthday = 25;
const int melody_happybirthday[] = {
    NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_C6, NOTE_B5,
    NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D6, NOTE_C6,
    NOTE_G5, NOTE_G5, NOTE_G5, NOTE_E6, NOTE_C6, NOTE_B5, NOTE_A5,
    NOTE_F6, NOTE_F6, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_C6

};
const int noteDurations_happybirthday[] = {
    8, 8, 4, 4, 4, 2,
    8, 8, 4, 4, 4, 2,
    8, 8, 4, 4, 4, 4, 4,
    8, 8, 4, 4, 4, 2
};

// softAP WIFI
const char *ssid = "med_reminder";
const char *password = "680271539";
//softAP server
AsyncWebServer server(80);

// Reminder List
const int REMINDER_SIZE = 10;
// arrays to store reminder data
String reminderName[REMINDER_SIZE];
long reminderTime[REMINDER_SIZE];
int reminderMusic[REMINDER_SIZE];
// Reminder List Prototypes
void initializeReminder();
int reminderFindIndexOf(String Mname);
int insertReminder(String Mname, long Mtime, int Mmusic);
void removeReminder(String Mname);
int setReminder(String Mname, long MwaitTime, int Mmusic);

// Function Prototypes
void setPin();
void setWifi();
void defineServerActions();
void checkDueTime();
void action();
void grafcet();
void blink_hint();
void playMusic(int music);

// global variables
int server_state = 0;
int initialized_PIN = 0;
int initialized_WIFI = 0;
int initialized_SERVER = 0;

// HTML and CSS
const char html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Medication Reminder Tool</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>

    <header>
        Medication Reminder Tool
    </header>

    <div class="container">

        <div class="form-container">
            <label for="medName">Medication Name</label>
            <input type="text" id="medName" placeholder="Enter medication name">

            <label for="reminderTime">Reminder Time</label>
            <input type="datetime-local" id="reminderTime" name="reminderTime" required>

            <label for="reminderSound">Reminder Sound</label>
            <select id="reminderSound">
                <option value=1>little star</option>
                <option value=2>merry christmas</option>
                <option value=3>happy birthday</option>
            </select>

            <button onclick="addReminder()">Add Reminder</button>
        </div>

        <table id="reminderTable">
            <thead>
                <tr>
                    <th>Medication Name</th>
                    <th>Reminder Time</th>
                    <th>Sound</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                <!-- Dynamic Reminder List will be injected here -->
            </tbody>
        </table>

    </div>

    <div class="footer">
        <p>ESP32 Medication Reminder Tool</p>
    </div>

    <script>

        window.onload = function() {
            setInterval(grafcet, 1000);
        };

        let client_state = 0;
        let add_btn = false;
        let del_btn = false;
        let indexOfReminder_del = -1;
        let input_error = false;
        let input_error_msg = "";

        function grafcet() {
            if (client_state == 0) {
                client_state = 1;
            }
            else if (client_state == 1) {
                if (add_btn) {
                    client_state = 2;
                    add_btn = false;
                }
                else if (del_btn) {
                    client_state = 3;
                    del_btn = false;
                }
                else {
                    client_state = 1;
                }
            }
            else if (client_state == 2) {
                if (input_error) {
                    client_state = 5;
                }
                else {
                    client_state = 4;
                }
                input_error = false;
            }
            else if (client_state == 3) {
                client_state = 1;
            }
            else if (client_state == 4) {
                client_state = 6;
            }
            else if (client_state == 5) {
                client_state = 1;
            }
            else if (client_state == 6) {
                client_state = 1;
            }
            else {}
            action();
        }

        function action() {
            if (client_state == 0) {
                initialize_page();
            }
            else if (client_state == 1) {
                updateReminderTable();
            }
            else if (client_state == 2) {
                checkInput();
            }
            else if (client_state == 3) {
                sendDeleteReminderRequest();
            }
            else if (client_state == 4) {
                sendAddReminderRequest();
            }
            else if (client_state == 5) {
                alert(input_error_msg);
            }
            else if (client_state == 6) {
                clearForm();
            }
            else {}
        }

        // Function to initialize the page
        function initialize_page() {
            // Function to prevent special characters in medication name input
            document.getElementById('medName').addEventListener('input', function() {
                this.value = this.value.replace(/[^a-zA-Z0-9 ]/g, '');
            });
        }

        // Function to update the reminder table
        function updateReminderTable() {
            fetch('/get_reminder', {
                method: 'GET',
            })
            .then(response => response.json())
            .then(data => setReminderList(data));
            const reminderList = getReminderList();
            const tableBody = document.getElementById('reminderTable').getElementsByTagName('tbody')[0];
            tableBody.innerHTML = '';
            reminderList.forEach((reminder, index) => {
                const row = document.createElement('tr');
                reminderTime_str = getFutureTime(parseInt(reminder.reminderTime));
                reminderSound_type = parseInt(reminder.reminderSound);
                reminderSound_str = reminderSound_type == 1 ? 'little star' : reminderSound_type == 2 ? 'merry christmas' : 'happy birthday';
                row.innerHTML = `
                    <td>${reminder.medName}</td>
                    <td>${reminderTime_str}</td>
                    <td>${reminderSound_str}</td>
                    <td><button class="delete-btn" onclick="deleteReminder(${index})">Delete</button></td>
                `;
                tableBody.appendChild(row);
            });
        }

        function addReminder() {
            add_btn = true;
        }

        function deleteReminder(index) {
            indexOfReminder_del = index;
            del_btn = true;
        }

        function checkInput() {
            const medName = document.getElementById('medName').value;
            const reminderTime = document.getElementById('reminderTime').value;
            const reminderSound = document.getElementById('reminderSound').value;
            if (medName && reminderTime && reminderSound) {
                if (medName.length > 20) {
                    input_error_msg = "Error: medication name is too long";
                    input_error = true;
                    return;
                }
                waitTime = (new Date(reminderTime).getTime() - new Date().getTime()) / 1000;
                if (waitTime < 0) {
                    input_error_msg = "Error: Reminder time cannot be in the past.";
                    input_error = true;
                    return;
                }
                else if (waitTime > 7*24*60*60) {
                    input_error_msg = "Error: Reminder time is more than 7 days in the future.";
                    input_error = true;
                    return;
                }
                input_error_msg = "No error: input data complete";
                input_error = false;
                return;
            } else {
                input_error_msg = "Error: input data incomplete. Please fill in all fields.";
                input_error = true;
                return;
            }
        }

        // Function to send "add reminder request" to server
        function sendAddReminderRequest() {
            const medName = document.getElementById('medName').value;
            const reminderTime = document.getElementById('reminderTime').value;
            setReminderTime = (new Date(reminderTime).getTime() - new Date().getTime()) / 1000;
            const reminderSound = document.getElementById('reminderSound').value;
            fetch('/set_reminder', {
                method: 'POST',
                body:
                  new URLSearchParams({
                    'action': 'add',
                    'medName': medName,
                    'reminderTime': setReminderTime,
                    'selectedMusic': reminderSound
                  }),
            })
            .then(response => response.text())
            .then(data => check(data));
        }

        // Function to send "delete reminder request" to server
        function sendDeleteReminderRequest() {
            const reminderList = getReminderList();
            medname = reminderList[indexOfReminder_del].medName;
            fetch('/set_reminder', {
                method: 'POST',
                body:
                  new URLSearchParams({
                    'action': 'remove',
                    'medName': medname,
                    'reminderTime': 0,
                    'selectedMusic': 0
                  }),
            })
            .then(response => response.text())
            .then(data => check(data));
            indexOfReminder_del = -1;
        }

        // Function to clear form inputs
        function clearForm() {
            document.getElementById('medName').value = '';
            document.getElementById('reminderTime').value = '';
            document.getElementById('reminderSound').value = 1;
        }

        // Function to get future time
        function getFutureTime(waitTime) {
            const now = new Date();
            now.setSeconds(now.getSeconds() + waitTime);
  
            const dayOfWeek = now.toLocaleString('en-us', { weekday: 'long' });
            const year = now.getFullYear();
            const month = String(now.getMonth() + 1).padStart(2, '0');
            const date = String(now.getDate()).padStart(2, '0');
            const hours = String(now.getHours()).padStart(2, '0');
            const minutes = String(now.getMinutes()).padStart(2, '0');

            return `${dayOfWeek}, ${year}/${month}/${date}, ${hours}:${minutes}`;
        }

        // Function to check response data
        function check(data) {
            if (data === 'Cannot set reminder. Reminder list is full.') {
                alert('Cannot set reminder. Reminder list is full.');
            }
            else if (data === 'Reminder set successfully.') {
                alert(data);
            }
            else if (data === 'Reminder removed successfully.') {
                alert(data);
            }
            else {
                alert(data);
            }
        }

        // Get reminder list from session storage
        function getReminderList() {
            const reminderList = sessionStorage.getItem('reminderList');
            return reminderList ? JSON.parse(reminderList) : [];
        }

        // Set reminder list to session storage
        function setReminderList(reminderList) {
            sessionStorage.setItem('reminderList', JSON.stringify(reminderList));
        }

    </script>

</body>
</html>

)rawliteral";

const char css[] PROGMEM = R"rawliteral(
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    background-color: #f4f4f9;
}

header {
    background-color: #4CAF50;
    color: white;
    text-align: center;
    padding: 15px 0;
    font-size: 24px;
}

.container {
    padding: 20px;
}

.form-container {
    background-color: white;
    padding: 15px;
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
    margin-bottom: 20px;
}

label {
    display: block;
    margin-bottom: 8px;
    font-weight: bold;
}

input, select {
    width: 100%;
    padding: 8px;
    margin: 5px 0 15px 0;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
}

button {
    background-color: #4CAF50;
    color: white;
    padding: 10px 15px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
}

button:hover {
    background-color: #45a049;
}

table {
    width: 100%;
    border-collapse: collapse;
    margin-top: 20px;
}

th, td {
    padding: 10px;
    text-align: left;
    border: 1px solid #ddd;
}

th {
    background-color: #4CAF50;
    color: white;
}

.delete-btn {
    background-color: #f44336;
    color: white;
    padding: 5px 10px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

.delete-btn:hover {
    background-color: #e53935;
}

.footer {
    text-align: center;
    padding: 10px;
    background-color: #4CAF50;
    color: white;
    position: fixed;
    width: 100%;
    bottom: 0;
}

)rawliteral";

void setup() {
  server_state = 0;
  initialized_PIN = 0;
  initialized_WIFI = 0;
  initialized_SERVER = 0;
}

void loop() {
  grafcet();
}

void blink_hint() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(BUILTIN_LED, LOW);
    delay(200);
  }
}

void playMusic(int music) {
  switch(music) {
    case 1:
      for (int thisNote = 0; thisNote < size_littlestar; thisNote++) {
        int noteDuration = 1000 / noteDurations_littlestar[thisNote];
        tone(BUZZER, melody_littlestar[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.40;
        delay(pauseBetweenNotes);
        noTone(BUZZER);
      }
      break;
    case 2:
      for (int thisNote = 0; thisNote < size_merrychristmas; thisNote++) {
        int noteDuration = 1000 / noteDurations_merrychristmas[thisNote];
        tone(BUZZER, melody_merrychristmas[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.40;
        delay(pauseBetweenNotes);
        noTone(BUZZER);
      }
      break;
    case 3:
      for (int thisNote = 0; thisNote < size_happybirthday; thisNote++) {
        int noteDuration = 1000 / noteDurations_happybirthday[thisNote];
        tone(BUZZER, melody_happybirthday[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.40;
        delay(pauseBetweenNotes);
        noTone(BUZZER);
      }
      break;
    default:
      break;
  }
}

void initializeReminder() {
  for (int i = 0; i < REMINDER_SIZE; i++) {
    reminderName[i] = "";
    reminderTime[i] = -1;
    reminderMusic[i] = -1;
  }
}
int reminderFindIndexOf(String Mname) {
  for (int i = 0; i < REMINDER_SIZE; i++) {
    if (reminderName[i] == Mname) {
      return i;
    }
  }
  return -1;
}
int insertReminder(String Mname, long Mtime, int Mmusic) {
  int index = reminderFindIndexOf(Mname);
  // If reminder name does not exist in the list
  if (index == -1) {
    for (int i = 0; i < REMINDER_SIZE; i++) {
      if (reminderName[i] == "") {
        reminderName[i] = Mname;
        reminderTime[i] = Mtime;
        reminderMusic[i] = Mmusic;
        return i;
      }
    }
    return -1;
  }
  // If reminder name exists in the list, update time and music
  else {
    reminderTime[index] = Mtime;
    reminderMusic[index] = Mmusic;
    return index;
  }
}
void removeReminder(String Mname) {
  int index = reminderFindIndexOf(Mname);
  if (index != -1) {
    reminderName[index] = "";
    reminderTime[index] = -1;
    reminderMusic[index] = -1;
  }
}
int setReminder(String Mname, long MwaitTime, int Mmusic) {
  long currentTime = millis();
  long reminderTime = currentTime + MwaitTime*1000;
  int index = insertReminder(Mname, reminderTime, Mmusic);
  if (index == -1) {
    Serial.println("Cannot set reminder. Reminder list is full.");
  }
  else {
    Serial.print("medicine reminder: ");
    Serial.print(Mname);
    Serial.print(", set at ");
    Serial.print(reminderTime);
    Serial.print(", with music ");
    Serial.print(Mmusic);
    Serial.println(".");
  }
  return index;
}

void setPin() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  noTone(BUZZER);
  initialized_PIN = 1;
}
void setWifi() {
  WiFi.softAP(ssid, password);
  Serial.println("SoftAP Started");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("SoftAP IP Address: ");
  Serial.println(IP);
  initialized_WIFI = 1;
}
void defineServerActions() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/css", css);
  });

  server.on("/get_reminder", HTTP_GET, [](AsyncWebServerRequest *request){
    String reminderList = "[";
    for (int i = 0; i < REMINDER_SIZE; i++) {
      if (reminderName[i] != "") {
        reminderList += "{\"medName\":\"" + reminderName[i] + "\",\"reminderTime\":\"" + String((reminderTime[i]-millis())/1000) + "\",\"reminderSound\":\"" + String(reminderMusic[i]) + "\"},";
      }
    }
    if (reminderList.length() > 1) {
      reminderList = reminderList.substring(0, reminderList.length()-1);
    }
    else {
      reminderList = reminderList.substring(0, reminderList.length());
    }
    reminderList += "]";
    request->send(200, "application/json", reminderList);
    Serial.println("Reminder list sent.");
    Serial.println(reminderList);
  });

  server.on("/set_reminder", HTTP_POST, [](AsyncWebServerRequest *request){
    String medName = request->arg("medName");
    String reminderTime = request->arg("reminderTime");
    long waitTime = reminderTime.toInt();
    String selectedMusic = request->arg("selectedMusic");
    int music = selectedMusic.toInt();
    String action = request->arg("action");
    if (action == "add") {
      int set = setReminder(medName, waitTime, music);
      if (set == -1) {
        request->send(200, "text/plain", "Cannot set reminder. Reminder list is full.");
      }
      else {
        request->send(200, "text/plain", "Reminder set successfully.");
      }
    }
    else if (action == "remove") {
      removeReminder(medName);
      request->send(200, "text/plain", "Reminder removed successfully.");
    }
    else {
      request->send(200, "text/plain", "Invalid action.");
    }
  });

  server.begin();
  initialized_SERVER = 1;

}

void checkDueTime() {
  for (int i = 0; i < REMINDER_SIZE; i++) {
    if (reminderName[i] != "") {
      long currentTime = millis();
      if (currentTime >= reminderTime[i] && reminderTime[i] != -1) {
        Serial.print("Reminder name: ");
        Serial.print(reminderName[i]);
        Serial.print(" at ");
        Serial.print(reminderTime[i]);
        Serial.print(" with music ");
        Serial.print(reminderMusic[i]);
        Serial.println(" is due.");
        int theMusic = reminderMusic[i];
        removeReminder(reminderName[i]);
        blink_hint();
        playMusic(theMusic);
      }
    }
  }
}

void action() {
  if (server_state == 0) {
    setPin();
  }
  else if (server_state == 1) {
    setWifi();
  }
  else if (server_state == 2) {
    defineServerActions();
  }
  else if (server_state == 3) {
    checkDueTime();
  }
  else {}
}

void grafcet() {
  if (server_state == 0) {
    if (initialized_PIN) {
      server_state = 1;
    }
  }
  else if (server_state == 1) {
    if (initialized_WIFI) {
      server_state = 2;
    }
  }
  else if (server_state == 2) {
    if (initialized_SERVER) {
      server_state = 3;
    }
  }
  else if (server_state == 3) {
    server_state = 3;
  }
  else {}
  action();
}
