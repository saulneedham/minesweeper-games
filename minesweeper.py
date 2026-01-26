#Python - OOP

from random import randint as rdi

#---------------------------------------------------

def withinLimit(limitChecking, upperLimit):
    return 0 <= limitChecking < upperLimit

#SQUARES

class Square:
    def __init__(self):
        self.revealed = False
        self.flagged = False

    def flag(self):
        if not self.revealed:
            self.flagged = not self.flagged

    def reveal(self):
        self.revealed = True

    def __str__(self): #Function to print this rather than object
        if self.flagged:
            return 'F'
        elif not self.revealed:
            return '.'
        else:
            return '' #Value returned by children

class EmptySquare(Square):
    def __init__(self, value=0):
        super().__init__()
        self.value = value #Adj mines

    def __str__(self):
        if super().__str__() != '':
            return super().__str__()
        else:
            return str(self.value)

class MineSquare(Square):
    def __str__(self):
        if super().__str__() != '':
            return super().__str__()
        else:
            return 'X'
        
#---------------------------------------------------
#BOARD

class Board:
    def __init__(self, rows, cols, numMines):
        self.rows, self.cols = rows, cols
        self.grid = []
        for r in range(rows):
            rowList = []
            for c in range(cols):
                rowList.append(EmptySquare())
            self.grid.append(rowList)

        self.placeMines(numMines)
        self.calculateSquareValues()

    def placeMines(self, numMines):
        placed = 0
        while placed < numMines:
            r = rdi(0, self.rows-1)
            c = rdi(0, self.cols-1)
            if not type(self.grid[r][c]) == MineSquare:
                self.grid[r][c] = MineSquare()
                placed += 1

    def calculateSquareValues(self):
        for r in range(self.rows):
            for c in range(self.cols):
                if type(self.grid[r][c]) == EmptySquare:
                    count = self.countAdjMines(r, c)
                    self.grid[r][c].value = count

    def countAdjMines(self, row, col):
        count = 0
        for rowOffset in [-1, 0, 1]:
            for colOffset in [-1, 0, 1]:
                neighborR = row + rowOffset
                neighborC = col + colOffset
                if withinLimit(neighborR, self.rows) and withinLimit(neighborC, self.cols):
                    if type(self.grid[neighborR][neighborC]) == MineSquare:
                        count += 1
        return count

    def display(self):
        print('    ', end='')
        for i in range(1, self.cols + 1):
            print(f" {i} ", end='')
        print()
        print()
        i = 1
        for row in self.grid:
            print(i, end='   ')
            i += 1
            for square in row:
                print(f" {square} ", end='')
            print()

    def floodReveal(self, startRow, startCol):
        stack = [(startRow, startCol)]
        
        while stack:
            row, col = stack.pop()
            square = self.grid[row][col]
            
            if square.revealed or square.flagged:
                continue
        
            square.reveal()
            if type(square) == EmptySquare and square.value == 0:
                for rowOffset in [-1, 0, 1]:
                    for colOffset in [-1, 0, 1]:
                        if rowOffset == 0 and colOffset == 0:
                            continue

                        neighborR = row + rowOffset
                        neighborC = col + colOffset
                        if withinLimit(neighborR, self.rows) and withinLimit(neighborC, self.cols):
                            neighbor = self.grid[neighborR][neighborC]
                            if not neighbor.revealed:
                                stack.append((neighborR, neighborC))

    def detonateBomb(self, row, col):
        for rowOffset in [-1, 0, 1]:
            for colOffset in [-1, 0, 1]:
                neighborR = row + rowOffset
                neighborC = col + colOffset
                if withinLimit(neighborR, self.rows) and withinLimit(neighborC, self.cols):
                    self.grid[neighborR][neighborC].reveal()

    def removeRow(self, row):
        self.grid.pop(row)
        self.rows -= 1
        self.calculateSquareValues()

    def winCheck(self):
        for row in self.grid:
            for square in row:
                if type(square) == EmptySquare and not square.revealed:
                    return False
        
        return True

#---------------------------------------------------
#MAIN

class Game:
    def __init__(self, rows, cols, numMines, bombsLeft, removesLeft):
        self.board = Board(rows, cols, numMines)
        self.running = True
        self.bombsLeft = bombsLeft
        self.removesLeft = removesLeft

    def run(self):
        while self.running:
            self.board.display()

            menu = (
                f"\n1) Uncover square\n"
                f"2) Flag/Unflag square\n"
                f"3) Place bomb (Bombs left: {self.bombsLeft})\n"
                f"4) Remove row (Removes left: {self.removesLeft})\n\n"
                "What would you like to do? (1,2,3,4) - "
            )

            choice = int(input(menu))

            if choice<4:
                #Uncover, flag, place bomb

                row = int(input("Enter row: ")) - 1
                column = int(input("Enter column: ")) - 1

                if not withinLimit(row, self.board.rows) or not withinLimit(column, self.board.cols):
                    print("Invalid coordinates!")
                    continue

                square = self.board.grid[row][column]

                if choice==1:
                    #1) UNCOVER SQUARE

                    if type(square) == MineSquare:
                        self.board.grid[row][column].reveal()
                        self.board.display()
                        print("Mine hit - you lose!")
                        self.running = False
                    else:
                        self.board.floodReveal(row, column)

                elif choice == 2:
                    #2) FLAG OR UNFLAG

                    square.flag()

                elif choice == 3:
                    #3) PLACE BOMB

                    if self.bombsLeft > 0:
                        self.board.detonateBomb(row, column)
                        self.bombsLeft -= 1
                    else:
                        print("No bombs left!")

            else:
                #4) REMOVE ROW

                row = int(input("Enter row to remove: ")) - 1

                if not withinLimit(row, self.board.rows):
                    print("Invalid row!")
                    continue

                if self.removesLeft > 0:
                    self.board.removeRow(row)
                    self.removesLeft -= 1
                else:
                    print("No removes left!")

            if self.board.winCheck(): #Checking after every action for win
                self.board.display()
                print("All squares uncovered - you win!")
                self.running = False

Game(9, 9, 10, 3, 1).run()
#rows, columns, mines, bombs, removes