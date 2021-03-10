### Better battery

A simple app to hibernate once the battery is 70-75% for a safe long storage. A lithium battery loses
its max capacity if put to storage fully charged. The best charge range is around 70-75%.

Notes: 
- does NOT drain battery irself. (TODO) Use other tools for that purpose.
- does NOT hibernate in case charger is connected. Emits sound and prints a message instead.
- does NOT hibernate if charged less than 70% and no charger is connected. Emits sound and prints
a message instead.

TODO:
- cross-compile for windows, maybe [dockerized](https://github.com/dockcross/dockcross))
- add some build and test pipeline to produce .exe
- drain battery from fully charged
