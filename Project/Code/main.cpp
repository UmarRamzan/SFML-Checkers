#include <iostream>
#include <SFML/Graphics.hpp>



class piece {

public:

    // create the circle object we'll use to display our piece
    sf::CircleShape circle;

    // get the boundries of our circle, to be used to detect mouse clicks
    sf::FloatRect boundingBox = circle.getGlobalBounds();

    // to tell which team this piece is on
    int m_teamNumber;

    // position of the piece on our 8x8 grid
    int grid_position[2];

    // check for when a piece is deleted
    bool deleted = false;

    // check if the piece has reached the other side
    bool kinged = false;

    // the length of the boxes surrounding our pieces
    int m_length;

    int m_radius;

    void selectTeam(int teamNumber) {

        m_teamNumber = teamNumber;

        // set color according to team number
        if (teamNumber == 1) {
            circle.setFillColor(sf::Color::White);
            circle.setOutlineColor(sf::Color::Black);
            circle.setOutlineThickness(5);
        }
        else if (teamNumber == 2) {
            circle.setFillColor(sf::Color::Black);
            circle.setOutlineColor(sf::Color::Color(245, 245, 245));
            circle.setOutlineThickness(5);
        }

    }

    // set the radius of our circle
    void setRadius(float radius) {
        m_radius = radius;
        circle.setRadius(radius);
    }

    void setGridPosition(int i, int j) {
        grid_position[0] = i;
        grid_position[1] = j;
    }

    // set the position of our circle in terms of pixels
    void setPosition(sf::Vector2f pos) {
        circle.setOrigin(m_radius, m_radius);
        circle.setPosition(pos);
    }

    // drow our circle and update its boundries
    void draw(sf::RenderWindow& target) {
        target.draw(circle);
    }

    // returns the current position of our circle
    sf::Vector2f getPosition() {
        return circle.getPosition();
    }

    // returns the current boundry of our circle
    sf::FloatRect getBoundry() {
        return circle.getGlobalBounds();
    }

    void init(int teamNumber, int pos_i, int pos_j, int length, int radius) {

        m_length = length;

        selectTeam(teamNumber);
        setRadius(radius);

        // calculate the offset required to place the circle in the centre of our squares
        int offset = (m_length / 2);

        setPosition(sf::Vector2f(pos_j * m_length + offset, pos_i * m_length + offset));
        setGridPosition(pos_i, pos_j);
    }

    // function to check the validity of any move, takes coordinates of our piece from the grid, and pointers to the grid, as inputs
    void check_move(int final_position[], int* positions, int* pieces, int* timer) {

        // copy positions grid to a temp array to make it easier to use
        int temp_positions[8][8];

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                temp_positions[i][j] = *((positions + i * 8) + j);
            }
        }

        // extract i and j coordinates from position arrays
        int initial_position_i = grid_position[0];
        int initial_position_j = grid_position[1];

        int final_position_i = final_position[0];
        int final_position_j = final_position[1];

        // calculate horizontal and vertical distance
        int horizontal_distance = final_position_j - initial_position_j;
        int vertical_distance = initial_position_i - final_position_i;

        // check to see if move is valid
        bool valid = false;

        // check to see if a piece has to be removed
        bool remove_piece = false;

        // check that the final location is empty
        if (temp_positions[final_position_i][final_position_j] == 0) {

            // for a travelled distance of 1 in each direction
            if (horizontal_distance == 1 || horizontal_distance == -1) {

                // if kinged no need to check for direction
                if (kinged) {
                    if ((vertical_distance == 1 || vertical_distance == -1)) {
                        valid = true;
                    }
                }
                // otherwise check direction according to team
                else {
                    if ((m_teamNumber == 1 && vertical_distance == 1) || (m_teamNumber == 2 && vertical_distance == -1)) {
                        valid = true;
                    }
                }

            }

            // for a travelled distance of 2 in each direction
            if (horizontal_distance == 2 || horizontal_distance == -2) {

                if (kinged) {

                    if ((vertical_distance == 2 || vertical_distance == -2)) {

                        int opponentTeamNumber;
                        if (m_teamNumber == 1) { opponentTeamNumber = 2; }
                        else { opponentTeamNumber = 1; }

                        // check if there is an enemy piece in between
                        int middle_distance_i = (initial_position_i + final_position_i) / 2;
                        int middle_distance_j = (initial_position_j + final_position_j) / 2;

                        if (temp_positions[middle_distance_i][middle_distance_j] == opponentTeamNumber) {

                            // signify that the middle piece has been removed
                            temp_positions[middle_distance_i][middle_distance_j] = -1;

                            valid = true;
                            remove_piece = true;
                        }
                    }
                }
                else if ((m_teamNumber == 1 && vertical_distance == 2) || (m_teamNumber == 2 && vertical_distance == -2)) {
                        
                        // find opponent team number
                        int opponentTeamNumber;
                        if (m_teamNumber == 1) { opponentTeamNumber = 2; }
                        else { opponentTeamNumber = 1; }

                        // check if there is an enemy piece in between
                        int middle_distance_i = (initial_position_i + final_position_i) / 2;
                        int middle_distance_j = (initial_position_j + final_position_j) / 2;

                        if (temp_positions[middle_distance_i][middle_distance_j] == opponentTeamNumber) {

                            // signify that the middle piece has been removed
                            temp_positions[middle_distance_i][middle_distance_j] = -1;
                            
                            valid = true;
                            remove_piece = true;
                        }
                    }
                }
            }

            

            if (valid) {
        
                // check if the piece has reached the other side of the board
                if (final_position_i == 0 && m_teamNumber == 1) { kinged = true;  }
                if (final_position_i == 7 && m_teamNumber == 2) { kinged = true;  }

                // change border color to signify kinged piece
                if (kinged) {
                    circle.setOutlineColor(sf::Color::Yellow);
                }

                // snap to position in the middle of the square
                int offset = (m_length / 2);
                sf::Vector2f final_position = sf::Vector2f(final_position_j * m_length + offset, final_position_i * m_length + offset);
                setPosition(final_position);

                // update temp grid
                temp_positions[initial_position_i][initial_position_j] = 0;
                temp_positions[final_position_i][final_position_j] = m_teamNumber;

                // update piece grid information
                grid_position[0] = final_position_i;
                grid_position[1] = final_position_j;

                // used to see if another turn should be added
                int possible_captures = 0;

                // pause the timer if a piece has been captured, allowing an extra turn
                if (remove_piece) {

                    int opponentTeamNumber;
                    if (m_teamNumber == 1) { opponentTeamNumber = 2; }
                    else { opponentTeamNumber = 1; }

                    // check if there is another piece to be captured
                    if (kinged) {

                        if (temp_positions[final_position_i + 2 / 2][final_position_j + 2 / 2] == opponentTeamNumber) {
                            if (temp_positions[final_position_i + 2][final_position_j + 2] == 0) {
                                int possible_captue_i = final_position_i + 2;
                                int possible_captue_j = final_position_j + 2;

                                if ((possible_captue_i <= 7 && possible_captue_i >= 0) && (possible_captue_j <= 7 && possible_captue_j >= 0)) {
                                    possible_captures++;
                                }
                            }
                        }

                        if (temp_positions[final_position_i + 2 / 2][final_position_j - 2 / 2] == opponentTeamNumber) {
                            if (temp_positions[final_position_i + 2][final_position_j - 2] == 0) {
                                int possible_captue_i = final_position_i + 2;
                                int possible_captue_j = final_position_j - 2;

                                if ((possible_captue_i <= 7 && possible_captue_i >= 0) && (possible_captue_j <= 7 && possible_captue_j >= 0)) {
                                    possible_captures++;
                                }
                            }
                        }

                        if (temp_positions[final_position_i - 2 / 2][final_position_j + 2 / 2] == opponentTeamNumber) {
                            if (temp_positions[final_position_i - 2][final_position_j + 2] == 0) {
                                int possible_captue_i = final_position_i - 2;
                                int possible_captue_j = final_position_j + 2;

                                if ((possible_captue_i <= 7 && possible_captue_i >= 0) && (possible_captue_j <= 7 && possible_captue_j >= 0)) {
                                    possible_captures++;
                                }
                            }
                        }

                        if (temp_positions[final_position_i - 2 / 2][final_position_j - 2 / 2] == opponentTeamNumber) {
                            if (temp_positions[final_position_i - 2][final_position_j - 2] == 0) {
                                int possible_captue_i = final_position_i - 2;
                                int possible_captue_j = final_position_j - 2;

                                if ((possible_captue_i <= 7 && possible_captue_i >= 0) && (possible_captue_j <= 7 && possible_captue_j >= 0)) {
                                    possible_captures++;
                                }
                            }
                        }


                    }

                    //for non kinged pieces
                    else {



                        int vertical = 0;

                        if (m_teamNumber == 1) {
                            vertical = -1;
                        }
                        else {
                            vertical = 1;
                        }

                        if (temp_positions[final_position_i + vertical][final_position_j + 1] == opponentTeamNumber) {
                            if (temp_positions[final_position_i + 2 * vertical][final_position_j + 2] == 0) {
                                int possible_captue_i = final_position_i + 2 * vertical;
                                int possible_captue_j = final_position_j + 2;

                                std::cout << possible_captue_i << " , " << possible_captue_j << std::endl;

                                if ((possible_captue_i <= 7 && possible_captue_i >= 0) && (possible_captue_j <= 7 && possible_captue_j >= 0)) {
                                    possible_captures++;
                                }
                            }
                        }
                        if (temp_positions[final_position_i + vertical][final_position_j - 1] == opponentTeamNumber) {
                            if (temp_positions[final_position_i + 2 * vertical][final_position_j - 2] == 0) {
                                int possible_captue_i = final_position_i + 2 * vertical;
                                int possible_captue_j = final_position_j - 2;

                                std::cout << possible_captue_i << " , " << possible_captue_j << std::endl;

                                if ((possible_captue_i <= 7 && possible_captue_i >= 0) && (possible_captue_j <= 7 && possible_captue_j >= 0)) {
                                    possible_captures++;
                                }
                            }
                        }



                    }
                }

                // pause timer if a piece has been captured and another piece is available for capture
                if (possible_captures == 0) {
                    *timer = *timer + 1;
                }

                // update orignal grid
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        *((positions + i * 8) + j) = temp_positions[i][j];
                    }
                }
            }
 
            else {

                // if move is invalid return piece to starting position
                int offset = (m_length / 2);
                sf::Vector2f orignal_position = sf::Vector2f(initial_position_j * m_length + offset, initial_position_i * m_length + offset);
                setPosition(orignal_position);
            }

        };
    };


void restartScreen(sf::RenderTarget& target) {
    target.clear(sf::Color::Black);
}

void mainLoop() {

    

    //create the window
    sf::RenderWindow window(sf::VideoMode(960, 960), "Checkers", sf::Style::Titlebar | sf::Style::Close);


    //---------------Initialize the board---------------------//

    // create turn timer and variables to track the win state
    int turn_timer = 0;
    int winner = 0;
    bool game_over = false;


    // create the board (a 2d array of rectangles)
    sf::RectangleShape Board_squares[8][8];

    // create the pieces (an vector containing two vectors, one for each player)
    piece pieces[2][12];

    int num_one_pieces = 12;
    int num_two_pieces = 12;

    // create a 2d array to help initialize the positions of our pieces
    int positions[8][8] = {

        {0,2,0,2,0,2,0,2},
        {2,0,2,0,2,0,2,0},
        {0,2,0,2,0,2,0,2},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {1,0,1,0,1,0,1,0},
        {0,1,0,1,0,1,0,1},
        {1,0,1,0,1,0,1,0},

    };

    // used when looping through our piece array
    int piece_number_1 = 0;
    int piece_number_2 = 0;

    // define the length of our squares
    int square_length = 120;

    for (int i = 0; i < 8; i++) {

        // tempColor is used to swicth between the two colors of our board
        bool tempColor;

        // alternate starting color between rows
        if (i % 2 == 0) { tempColor = true; }
        else { tempColor = false; }

        for (int j = 0; j < 8; j++) {

            // configure our rectangles to form a checkered board
            Board_squares[i][j].setPosition(sf::Vector2f(j * square_length, i * square_length));
            Board_squares[i][j].setSize(sf::Vector2f(square_length, square_length));
            Board_squares[i][j].setFillColor(tempColor ? sf::Color::Color(240, 217, 181) : sf::Color::Color(181, 136, 99));

            // alternate color across columns
            tempColor = !tempColor;

            // create a piece for team 1
            if (positions[i][j] == 1) {

                pieces[0][piece_number_1].init(1, i, j, square_length, 40);

                piece_number_1++;

                //create a piece for team 2
            }
            else if (positions[i][j] == 2) {

                pieces[1][piece_number_2].init(2, i, j, square_length, 40);

                piece_number_2++;

            }

        }

    }



    //---------------------WINDOW LOOP---------------------//

    // index values used to remember which piece is selected
    bool piece_selected = false;
    int piece_index_i;
    int piece_index_j;

    // run the program as long as the window is open
    while (window.isOpen())
    {

        // check if the game is over
            if (num_one_pieces == 0) {
                std::cout << "Player 2 wins" << std::endl;
                game_over = true;
            }
            else if (num_two_pieces == 0) {
                std::cout << "Player 1 wins" << std::endl;
                game_over = true;
            }

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))

            // execute the following code if mouse button is pressed
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                // if no piece is currently selected
                if (!piece_selected) {

                    // loop through piece array to see if the mouse has clicked within their boundries
                    for (int i = 0; i < 2; i++) {

                        for (int j = 0; j < 12; j++) {

                            if (pieces[i][j].getBoundry().contains(mousePosition.x, mousePosition.y))
                            {

                                if ((turn_timer % 2 == 0 && i == 0) || (turn_timer % 2 == 1 && i == 1)) {

                                    pieces[i][j].setPosition(sf::Vector2f(mousePosition));

                                    // remember which piece is currently selected
                                    piece_selected = true;
                                    piece_index_i = i;
                                    piece_index_j = j;

                                    // exit loop if a piece has been selected
                                    break;
                                }
                            }
                        }
                    }
                }

                else if (piece_selected) {
                    // continue to set pointer position to mouse position as long as the button is clicked
                    pieces[piece_index_i][piece_index_j].setPosition(sf::Vector2f(mousePosition));

                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {

                // compute movement if a piece had actually been selected, otherwise do nothing
                if (piece_selected) {
                    // find release posiiton of piece and check if move is valid
                    sf::Vector2f finalPosition = pieces[piece_index_i][piece_index_j].getPosition() / (float)square_length;
                    int final_grid_pos[] = { std::floor(finalPosition.y), std::floor(finalPosition.x) };

                    pieces[piece_index_i][piece_index_j].check_move(final_grid_pos, (int*)positions, (int*)pieces, &turn_timer);
                }

                piece_selected = false;
            }
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }



        // clear the window with black color
        window.clear(sf::Color::Black);


        // adding the delete property to pieces that have been removed
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 12; j++) {
                if (positions[pieces[i][j].grid_position[0]][pieces[i][j].grid_position[1]] == -1) {
                    positions[pieces[i][j].grid_position[0]][pieces[i][j].grid_position[1]] = 0;

                    for (int x = j; x < 11; x++) {
                        pieces[i][x] = pieces[i][x + 1];
                    }

                    if (i == 0) { num_one_pieces--; }
                    else { num_two_pieces--; }


                }
            }
        }


        // Drawing

        // the game board
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                window.draw(Board_squares[i][j]);
            }

        }

        // the pieces
        for (int i = 0; i < num_one_pieces; i++) {
            pieces[0][i].draw(window);
        }
        for (int i = 0; i < num_two_pieces; i++) {
            pieces[1][i].draw(window);
        }

        // end the current frame
        window.display();
    }
}

int main()
{
    mainLoop();

    return 0;

};

