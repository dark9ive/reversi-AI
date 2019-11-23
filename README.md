# Reversi-AI

This is a project of multiple AI Algorithms on a single game -- **Reversi**  

Each algorithm will be in a independent branch.  

## Game rule

WIP

## Branch Master

The Master branch will be the easiest -- the game itself.  
Nothing much to say, only works for local dual-player.

### Install and Play

Download the Master branch by using the following command:

```
git clone --single-branch --branch master https://github.com/dark9ive/reversi-AI.git && cd ./reversi-AI
```

Then you can compile the code with the following command:

```
g++ ./main.cpp -o whatever_name_you_want
```

Finally, execute the file with:

```
./whatever_name_you_want <side_lenth_of_the_board>
```

In each step, you must insert two coordinates -- X(↓), Y(→) in **numbers** and separate them with a space  
If you try to put on a prohibit coordinates, your move will be returned.  
If the board is full, the game ends.  
Have fun!  
