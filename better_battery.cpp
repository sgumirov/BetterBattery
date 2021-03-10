#include <iostream>
#include <windows.h>
#include <unistd.h>

using namespace std;

BOOL GetPrivilege();

int main(int argc, char *argv[]) {
    SYSTEM_POWER_STATUS spsPwr;
    BYTE battery_percentage = 0, battery_percentage_old = 0;
    bool failed = false, charging = false;
    cout << "**********************************************************************************************************************" << endl;
    cout << "* Better_Battery: Simple app to wait until battery is in range 70-75% and then emit sound or hibernate if NOT charging" << endl;
    cout << "**********************************************************************************************************************" << endl;
    do {
        sleep(1);
        if( GetSystemPowerStatus(&spsPwr) ) {
            battery_percentage = spsPwr.BatteryLifePercent;
            charging = (spsPwr.ACLineStatus && (spsPwr.BatteryFlag & 8) == 8);
            if (battery_percentage_old != battery_percentage) {
                cout << "AC Status : "      << static_cast<double>(spsPwr.ACLineStatus)
                     << "\nBattery Status : " << static_cast<double>(spsPwr.BatteryFlag)
                     << "\nBattery Life % : " << static_cast<double>(battery_percentage)
                     << "\nCharging : "       << (charging ? "true":"false")
                     << endl;
                battery_percentage_old = battery_percentage;

                // Sound when it's almost time to hibernate
                if (!charging && battery_percentage == 76 || battery_percentage == 77) {
                    MessageBeep(MB_ICONEXCLAMATION);
                    cout << "Almost there!" << endl;
                }
            }
        } else {
            failed = true;
        }
    } while ( !failed && 
              (battery_percentage < 70 && charging || 
               battery_percentage > 75 && !charging)
            );

    if (failed) {
        cout << "ERROR: battery data fetch failed." << endl;
        MessageBeep(MB_ICONEXCLAMATION);
        return 1;
    }

    if (battery_percentage > 75 && charging || battery_percentage < 70 && !charging) {
        MessageBeep(MB_ICONEXCLAMATION);
        cout << (battery_percentage > 75 ? "Battery charge more than needed. Please disconnect" : 
                                           "Battery charge is less than desired. Please connect") <<
                " the charger first and run Better_Battery again." << endl;
        return 1;
    }

    if (!charging) {
        MessageBeep(MB_ICONWARNING);
        cout << "Battery charge is in desired range. Hibernating in 5 seconds..." << endl;
        sleep(5);
        if (GetPrivilege()) {
            if (SetSystemPowerState(FALSE, TRUE) == 0) {
                MessageBeep(MB_ICONERROR);
                cout << "ERROR: Failed to hibernate! Please HIBERNATE manually.";
                return 2;
            } else {
                return 0;
            }
        } else {
            MessageBeep(MB_ICONERROR);
            cout << "ERROR: Process does not have privileges to hibernate" << endl;
        }
    }

    MessageBeep(MB_OK);
    cout << "You may DISCONNECT ac power AND HIBERNATE now." << endl;
    return 0;
}

BOOL GetPrivilege()
{
    HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 
  
    // Get a token for this process. 
  
    if (!OpenProcessToken(GetCurrentProcess(), 
         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
       return FALSE;
  
    // Get the LUID for the shutdown privilege. 
  
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
         &tkp.Privileges[0].Luid); 
  
    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
  
    // Get the shutdown privilege for this process. 
  
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
         (PTOKEN_PRIVILEGES)NULL, 0); 
  
    if (GetLastError() != ERROR_SUCCESS) 
       return FALSE;
  
    return TRUE;
}
