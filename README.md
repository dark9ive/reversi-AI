# Reversi-AI

This is a project of multiple AI Algorithms on a single game -- **Reversi**  

Each algorithm will be in a independent branch.  

## Game rule

[Game Rules](https://en.wikipedia.org/wiki/Reversi#Rules)

## Branch training

In this branch is the version using Temporal Difference learning  

### Install and Play

Download the Master branch by using the following command:

```
git clone --single-branch --branch training https://github.com/dark9ive/reversi-AI.git && cd ./reversi-AI
```

Then you can compile the code with the following command:

```
g++ ./main.cpp -o whatever_name_you_want
```

Finally, execute the file with:

```
./whatever_name_you_want <side_lenth_of_the_board> <mode_code> <color_code(if necessary)>
```
modecode:  
 - 0 for playing with the other player.
 - 1 for training AI.
 - 2 for playing with AI(need color code).
  
colorcode:
 - 0 for playing white.
 - 1 for playing black.
  
In each step, you must insert two coordinates -- X(↓), Y(→) in **numbers** and separate them with a space  
If you try to put on a prohibit coordinates, your move will be returned.  
Have fun!  
