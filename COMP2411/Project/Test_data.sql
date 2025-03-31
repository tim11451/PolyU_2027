-- Table for Banquets
CREATE TABLE Banquets
(
    BIN                NUMBER        NOT NULL,
    banquet_name       VARCHAR2(255) NOT NULL,
    date_time          VARCHAR2(255) NOT NULL,
    address            VARCHAR2(255) NOT NULL,
    location           VARCHAR2(255) NOT NULL,
    contact_first_name VARCHAR2(100) NOT NULL,
    contact_last_name  VARCHAR2(100) NOT NULL,
    available          CHAR(1) CHECK (available IN ('Y', 'N')),
    quota              NUMBER        NOT NULL,
	PRIMARY KEY(BIN)
);

-- Table for Meals
CREATE TABLE Meals
(
    meal_id         NUMBER        NOT NULL,
    BIN             NUMBER        NOT NULL,
    type            VARCHAR2(50)  NOT NULL,
    dish_name       VARCHAR2(255) NOT NULL,
    price           NUMBER(10, 2) NOT NULL,
    special_cuisine VARCHAR2(255),
	PRIMARY KEY (meal_id,BIN),
    FOREIGN KEY (BIN) REFERENCES Banquets (BIN)
);

-- Table for Attendees
CREATE TABLE Attendees
(
    email                   VARCHAR2(50)        NOT NULL,
    first_name              VARCHAR2(20)        NOT NULL,
    last_name               VARCHAR2(20)        NOT NULL,
    address                 VARCHAR2(100)        NOT NULL,
    attendee_type           VARCHAR2(20)         NOT NULL,
    password                VARCHAR2(20)        NOT NULL,
    mobile_number           CHAR(8),
    affiliated_organization VARCHAR2(50)         NOT NULL,
	PRIMARY KEY (Email)
);

-- Table for Registrations
CREATE TABLE Registrations
(
    registration_id   NUMBER NOT NULL,
    email             VARCHAR(50) NOT NULL,
    BIN               NUMBER NOT NULL,
    drink_choice      VARCHAR2(50),
    meal_choice       VARCHAR2(50),
    remarks           VARCHAR2(255),
    registration_time VARCHAR(50),
    seat_number       NUMBER,
    FOREIGN KEY (email) REFERENCES Attendees (email),
    FOREIGN KEY (BIN) REFERENCES Banquets (BIN),
    PRIMARY KEY(BIN,registration_id,email)
);


-- example for banquet
INSERT INTO Banquets (BIN, banquet_name, date_time, address, location, contact_first_name, contact_last_name, available, quota) 
VALUES (1, 'Annual Gala', '2024-12-01 18:00:00', '123 Celebration Ave, City', 'Grand Hall', 'John', 'Doe', 'Y', 100);

INSERT INTO Banquets (BIN, banquet_name, date_time, address, location, contact_first_name, contact_last_name, available, quota) 
VALUES (2, 'Charity Fundraiser', '2024-12-05 19:30:00', '456 Charity St, City', 'Ballroom A', 'Jane', 'Smith', 'Y', 150);

-- example for Attendee
INSERT INTO Attendees (email, first_name, last_name, address, attendee_type, password, mobile_number, affiliated_organization) 
VALUES ( 'helen.han@qq.com','Helen', 'HAN', 'hk', 'student', 'password', '12345678', 'PolyU');

INSERT INTO Attendees (email, first_name, last_name, address, attendee_type, password, mobile_number, affiliated_organization) 
VALUES ('michael.brown@example.com', 'Michael', 'Brown', '111 Main St, City', 'staff', 'password123', '12345678', 'PolyU');

INSERT INTO Attendees (email, first_name, last_name, address, attendee_type, password, mobile_number, affiliated_organization) 
VALUES ('james.taylor@example.com', 'James', 'Taylor', '333 Third St, City', 'alumni', 'mypassword789', '12345678', 'HKCC');

INSERT INTO Attendees (email, first_name, last_name, address, attendee_type, password, mobile_number, affiliated_organization) 
VALUES ('sara.lee@example.com', 'Sara', 'Lee', '444 Fourth St, City', 'staff', 'mypassword123', '87654321', 'HKUST');


-- example for registration
INSERT INTO Registrations (registration_id, email, BIN, drink_choice, meal_choice, remarks, registration_time, seat_number) 
VALUES (1, 'michael.brown@example.com', 1, 'Coke', 2, 'Looking forward to it!', '2024-11-01 12:00:00', 5);

INSERT INTO Registrations (registration_id, email, BIN, drink_choice, meal_choice, remarks, registration_time, seat_number) 
VALUES (2,'james.taylor@example.com', 1, 'Water',2, 'Please reserve a front seat.', '2024-11-01 12:05:00', 2);

INSERT INTO Registrations (registration_id, email, BIN, drink_choice, meal_choice, remarks, registration_time, seat_number) 
VALUES (3, 'james.taylor@example.com', 2, 'Juice', 6, 'Allergic to nuts.', '2024-11-01 12:10:00', 10);

INSERT INTO Registrations (registration_id, email, BIN, drink_choice, meal_choice,remarks, registration_time, seat_number)
VALUES (4, 'helen.han@qq.com',1,'Tea', 4,'Looking forward to it!', '2024-11-01 12:00:00', 5);



-- example for Meals
INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (1,1, 'Fish', 'Grilled Salmon', 25.50, 'Mediterranean');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (2,1, 'Fish', 'TangCu Fish', 50, 'Chinese');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (3, 1, 'Chicken', 'Roast Chicken', 8.75, 'French');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (4, 1, 'Chicken', 'Spicy Chicken', 80.00, 'Chinese');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (5, 2, 'Beef', 'Beef Salad', 12.00, 'Italian');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (6, 2, 'Beef', 'Roast Beef', 62.00, 'Italian');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (7, 2, 'Vegetarian', 'Goods for Vegetarian', 22.00, 'American');

INSERT INTO Meals (meal_id, BIN, type, dish_name, price, special_cuisine) 
VALUES (8, 2, 'Vegetarian', 'Fruit Salad', 22.00, 'Mexico');