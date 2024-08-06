## List of content

---

- **Info**
  
  - [Description](#Desciption)
  
  - [Features](#Features)
  
  - [Gallery & Video](#Non)

- **Installation**
  
  - [Using Git](#Using-Git)
  
  - [Binary releases](#Binary-Releases)

- **Usage**
  
  - [Hotkeys](#Hotkeys)
  
  - [Tips](#Tips)

- ***SAMPLE***

- **License**

## **Info**

### ***Description***

***Flags-to-SH***  is a TUI tool, which simplify creating custom scripts, refering to specified application with included flags
 
These project also implement [Ncurses API](https://invisible-island.net/ncurses/) as render framework.

### ***Features***

- Fast in usage

- TUI support

- Built-in attributes redactor

### *Gallery & Video*

## Installation

### ***Using Git***

The best way is to clone repository into your machine:

```shell
git clone https://github.com/FrenzyAsWell/Flags-To-SH.git && cd Flags-To-SH
```

#### ***Binary Releases***

Binary releases are avaliable from GitHub page:

- [All](https://github.com/FrenzyAsWell/Flags-To-SH/releases)
- [Latest](https://github.com/FrenzyAsWell/Flags-To-SH/releases/latest)

## Usage

Run ***Flags-to-SH*** wih name of executable, which you want to create scripts from:

```shell
flags-to-sh <exec-name>
```

### ***Hotkeys***

| Binds     | Desciption                          |
| --------- | ----------------------------------- |
| `j`or `↓` | **Move down** in from list of flags |
| `k`or `↑` | **Move up** in from list of flags   |
| `Enter`   | **Add to queue** or **select**      |
| `e`       | **Add value** to selected attribute |
| `y`       | **Save as** shell script            |

### ***Tips***

| Window       | Used for                                                     |
| ------------ | ------------------------------------------------------------ |
| `Flags`      | Displays currently avaliable flags to application            |
| `Desciption` | Gives a desciption of flag from `--help` command             |
| `Result`     | Displays end result, which will be written into shell script |

 

## License

[GNU GENERAL PUBLIC LICENSE](https://github.com/FrenzyAsWell/Flags-To-SH/blob/main/LICENSE) - Version 3, 29 June 2007

**2024** - *Yan Moiseev*


