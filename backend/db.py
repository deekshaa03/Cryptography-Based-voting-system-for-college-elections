import mysql.connector

def get_connection():
    connection = mysql.connector.connect(
        host="localhost",
        user="deeksha",            # change if needed
        password="Deeksha@123",    # change if needed
        database="votingdb"        # your database
    )
    return connection
