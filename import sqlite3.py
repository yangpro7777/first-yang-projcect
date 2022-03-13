import sqlite3
conn = sqlite3.connect('assembly.db')

cursor = conn.cursor()

cursor.execute('CREATE TABLE IF NOT EXISTS member(name text, code text')
members = [('홍길동', '12345'), ('김철수', '12345')]
cursor.executemany("INSERT INTO member VALUE (?,?)", members)

conn.commit() 
# db에 저장한다. 변화를 저장한다

for row in cursor.execute('SELECT * FROM members'):
    print(row)