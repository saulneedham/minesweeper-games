--Haskell - Functional

import System.Random

type SystemBoard = [Int]
type UserBoard   = [Char]

-- Functions for setting up board

placeMines :: Int -> [Int] -> IO [Int]
placeMines 0 grid = return grid
placeMines n grid = do
    idx <- randomRIO (0, 80) :: IO Int
    if grid !! idx == 0
        then placeMines (n - 1) (take idx grid ++ [-1] ++ drop (idx + 1) grid)
        else placeMines n grid

buildSystemBoard :: Int -> Int -> SystemBoard -> SystemBoard
buildSystemBoard i h sys
  | i == (h * 9) = []
  | getCell i sys == -1 = -1 : buildSystemBoard (i + 1) h sys
  | otherwise = countMines (i `div` 9) (i `mod` 9) sys : buildSystemBoard (i + 1) h sys
  where 
    countMines r c s = length [() | dr <- [-1..1], dc <- [-1..1], 
                               let nr = r+dr, let nc = c+dc, 
                               isValid nr nc h, getCell (getIndex nr nc) s == -1]

---------------------------------------------------------------------------------------

floodReveal :: Int -> Int -> Int -> SystemBoard -> UserBoard -> UserBoard
floodReveal rStart cStart h sys board = walk rStart cStart board
  where
    walk r c usr
      | not (isValid r c h) || getCell (getIndex r c) usr /= '.' = usr
      | val == -1 = usr
      | val > 0   = updateUser (getIndex r c) (toEnum (val + 48)) usr
      | otherwise = foldl (\acc (dr, dc) -> walk (r + dr) (c + dc) acc) 
                          (updateUser (getIndex r c) '0' usr) 
                          [(-1,-1), (-1,0), (-1,1), (0,-1), (0,1), (1,-1), (1,0), (1,1)]
      where val = getCell (getIndex r c) sys

detonateBomb :: Int -> Int -> Int -> SystemBoard -> UserBoard -> UserBoard
detonateBomb r c h sys usr = 
    foldl (\acc (dr, dc) -> 
        let nr = r + dr
            nc = c + dc
            idx = getIndex nr nc
        in if isValid nr nc h
           then let val = getCell idx sys
                    char = if val == -1 then 'X' else toEnum (val + 48)
                in updateUser idx char acc
           else acc
    ) usr [(-1,-1), (-1,0), (-1,1), (0,-1), (0,0), (0,1), (1,-1), (1,0), (1,1)]

getIndex :: Int -> Int -> Int
getIndex r c = r * 9 + c

isValid :: Int -> Int -> Int -> Bool
isValid r c h = r >= 0 && r < h && c >= 0 && c < 9

getCell :: Int -> [a] -> a
getCell 0 (x:xs) = x
getCell n (x:xs) = getCell (n - 1) xs

updateUser :: Int -> Char -> UserBoard -> UserBoard
updateUser 0 val (x:xs) = val : xs
updateUser n val (x:xs) = x : updateUser (n - 1) val xs

removeRow :: Int -> [a] -> [a]
removeRow row board = take (row * 9) board ++ drop ((row + 1) * 9) board

winCheck :: SystemBoard -> UserBoard -> Int -> Bool
winCheck sys usr h = not (elem True [ (usr!!i == '.' && sys!!i /= -1) | i <- [0..((h*9)-1)] ])

displayBoard :: UserBoard -> String
displayBoard [] = []
displayBoard xs = take 17 (intersperse ' ' (take 9 xs)) ++ "\n" ++ displayBoard (drop 9 xs)
  where intersperse c [] = []
        intersperse c [h] = [h]
        intersperse c (h:t) = h : c : intersperse c t

------------------------------------------------------------------------------------------------------------

gameLoop :: SystemBoard -> UserBoard -> Int -> Int -> Int -> IO ()
gameLoop sys usr h bLeft rLeft = do
    putStrLn (displayBoard usr)
    putStrLn "1) Uncover square\n2) Flag/Unflag square"
    putStrLn ("3) Place bomb (Bombs left: " ++ show bLeft ++ ")")
    putStrLn ("4) Remove row (Removes left: " ++ show rLeft ++ ")")
    putStrLn "\nWhat would you like to do? (1,2,3,4) - "
    choiceStr <- getLine
    let choice = read choiceStr :: Int

    if choice < 4 then do
        -- Uncover, flag, place bomb

        putStrLn ("Enter Row (1-" ++ show h ++ "):")
        rStr <- getLine
        let r = (read rStr :: Int) - 1
        putStrLn "Enter Column (1-9):"
        cStr <- getLine
        let c = (read cStr :: Int) - 1
        
        let idx = getIndex r c
        
        if not (isValid r c h) then do
            putStrLn "Invalid coordinates!"
            gameLoop sys usr h bLeft rLeft
        else case choice of
-- #1) UNCOVER SQUARE
            1 -> if getCell idx usr == 'F' then putStrLn "Square flagged!" >> gameLoop sys usr h bLeft rLeft 
                    else if getCell idx sys == -1 then do
                        putStrLn (displayBoard (updateUser idx 'X' usr))
                        putStrLn "Mine hit - you lose!"
                    else checkWin sys (floodReveal r c h sys usr) h bLeft rLeft
                    
-- #2) FLAG OR UNFLAG
            2 -> let nextUs = if getCell idx usr == 'F' then '.' else 'F'
                    in gameLoop sys (updateUser idx nextUs usr) h bLeft rLeft
                    
-- #3) PLACE BOMB
            3 -> if bLeft > 0 then checkWin sys (detonateBomb r c h sys usr) h (bLeft - 1) rLeft
                    else putStrLn "No bombs left!" >> gameLoop sys usr h bLeft rLeft
            _ -> gameLoop sys usr h bLeft rLeft
        
    else do
-- #4) REMOVE ROW

        putStrLn ("Enter Row index to remove (1-" ++ show h ++ "):")
        rStr <- getLine
        let r = (read rStr :: Int) - 1
        
        if rLeft > 0 && r >= 0 && r < h then do
            let newSys = removeRow r sys
            let newUsr = removeRow r usr
            -- Recalculate board state
            gameLoop (buildSystemBoard 0 (h - 1) newSys) newUsr (h - 1) bLeft (rLeft - 1)
        else do
            putStrLn "Invalid Row or No Removes Left!"
            gameLoop sys usr h bLeft rLeft

checkWin :: SystemBoard -> UserBoard -> Int -> Int -> Int -> IO ()
checkWin sys usr h b r = do
    if winCheck sys usr h then do
        putStrLn (displayBoard usr)
        putStrLn "All squares uncovered - you win!"
    else gameLoop sys usr h b r

main :: IO ()
main = do
    mines <- placeMines 10 (replicate 81 0)
    -- height: 9, mines: 3, removes: 1
    let systemBoard = buildSystemBoard 0 9 mines
    let userBoard = replicate 81 '.'
    gameLoop systemBoard userBoard 9 3 1