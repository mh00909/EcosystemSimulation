# Ecosystem Simulation

## Opis projektu

Projekt **Ecosystem Simulation** to symulacja ekosystemu napisana w C++. 
Symulacja przedstawia interakcje między różnymi organizmami, takimi jak roślinożercy, mięsożercy, padlinożercy i rośliny. 
Symulacja jest wyposażona w graficzny interfejs użytkownika (GUI) oparty na **Qt5**, który pozwala na interaktywne zarządzanie symulacją, modyfikowanie parametrów organizmów oraz obserwowanie ich zachowań.

### Organizmy w symulacji

- **Rośliny (Plant)**
Są konsumowane przez Roślinożerców (Herbivore).
Nie wchodzą w interakcje z mięsożercami ani padlinożercami.
Mogą być trujące (**PoisonousPlant**), powodują śmierć roślinożerców.
- **Roślinożercy (Herbivore)**
Zjadają rośliny, aby zwiększać energię. Są ofiarami mięsożerców (Carnivore) – stanowią źródło pożywienia dla drapieżników.
Mogą zostać zatruci przez Trujące Rośliny (PoisonousPlant).
- **Mięsożercy (Carnivore)**
Polują na roślinożerców, aby zdobyć energię.
- **Padlinożercy (Scavenger)**
Żywią się martwymi organizmami (roślinożercami, mięsożercami, a także innymi padlinożercami).

Organizmy mogą się rozmnażać, gdy mają dostateczną ilość energii i nie przekroczą określonego wieu.

## Technologie

- **C++** – główny język projektu.
- **Qt5** – biblioteka do tworzenia graficznego interfejsu użytkownika.
- **CMake** – narzędzie do konfiguracji budowania projektu.
