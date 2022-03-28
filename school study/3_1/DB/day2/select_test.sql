drop database if exists Madang;
create database Madang;
use Madang;
create table Book_table(bookid int auto_increment primary key,bookname varchar(20) not null,publisher varchar(20) not null,
price int not null);

insert into Book_table values(null,'축구의 역사','굿스포츠',7000);
insert into Book_table values(null,'축구아는 여자','나무수',13000);
insert into Book_table values(null,'축구의 이해','대한미디어',22000);
insert into Book_table values(null,'골프 바이블','대한미디어',35000);
insert into Book_table values(null,'피겨 교본','굿스포츠',8000);
insert into Book_table values(null,'역도 단계별기술','굿스포츠',6000);
insert into Book_table values(null,'야구의 추억','이상미디어',20000);
insert into Book_table values(null,'야구를 부탁해','이상미디어',13000);
insert into Book_table values(null,'올림픽 이야기','삼성당',7500);
insert into Book_table values(null,'Olympic Champions','Pearson',13000);

create table Customer_table(custid int auto_increment primary key,username varchar(20) not null,addr varchar(30) not null,
phone char(13));
insert into Customer_table values(null,'박지성','영국 맨체스타','000-5000-0001');
insert into Customer_table values(null,'김연아','대한민국 서울','000-6000-0001');
insert into Customer_table values(null,'장미란','대한민국 강원도','000-7000-0001');
insert into Customer_table values(null,'추신수','미국 클리블랜드','000-8000-0001');
insert into Customer_table values(null,'박세리','대한민국 대전',null);

create table Orders_table(orderid int auto_increment primary key,custid int not null,bookid int not null,saleprice int not null,
orderdate char(10) not null,foreign key(custid) references Customer_table(custid),foreign key(bookid) references Book_table(bookid));
insert into Orders_table values(null,1,1,6000,'2014-07-01');
insert into Orders_table values(null,1,3,21000,'2014-07-03');
insert into Orders_table values(null,2,5,8000,'2014-07-03');
insert into Orders_table values(null,3,6,6000,'2014-07-04');
insert into Orders_table values(null,4,7,20000,'2014-07-05');
insert into Orders_table values(null,1,2,12000,'2014-07-07');
insert into Orders_table values(null,4,8,13000,'2014-07-07');
insert into Orders_table values(null,3,10,12000,'2014-07-08');
insert into Orders_table values(null,2,10,7000,'2014-07-09');
insert into Orders_table values(null,3,8,13000,'2014-07-10');

-- 문제 1) 가격이 20000원 미만인 도서 검색
select * from book_table where price < 20000; 
-- 문제 2) 가격이 10000원 이상 20000 이하인 도서 검색
select * from book_table where price >=10000 and price <=20000;
-- 문제 3) 출판사가 '굿스포츠' 혹은 '대한미디어'인 도서를 검색
select * from book_table where publisher in ('굿스포츠','대한미디어');
-- 문제 4) 출판사가 '굿스포츠' 혹은 '대한미디어'가 아닌 도서를 검색
select * from book_table where publisher not in ('굿스포츠','대한미디어');
-- 문제 5) '축구의 역사'를 출간한 출판사를 검색
select publisher as 출판사 from book_table where bookname = '축구의 역사';
-- 문제 6) 도서이름에 '축구'가 포함된 출판사 검색
select publisher as 출판사 from book_table where bookname like '%축구%';
-- 문제 7) 도서이름이 여섯 글자인 도서를 검색
select * from book_table where bookname like '______';
-- 문제 8) 도서이름의 왼쪽 두 번째 위치에 '구'라는 문자열을 갖는 도서를 검색
select * from book_table where bookname like '_구%';
-- 문제 9) 축구에 관한 도서 중 가격이 20000원 이상인 도서를 검색
select * from book_table where bookname like '%축구%' and price>=20000;
-- 문제 10) 야구에 관한 책을 모두 구입하려면 필요한 금액 계산.
select sum(price) as 야구도서총금액 from book_table where bookname like '%야구%';
-- 문제 11) 도서를 가격 순으로 검색하고 가격이 같으면 이름순으로 검색
select * from book_table order by price asc ,bookname asc;
-- 문제 12) 도서를 가격의 내림차순으로 검색하고 만약 가격이 같다면 출판사의 오름차순으로 검색
select * from book_table order by price desc,publisher asc;
-- 문제 13) 주소가 우리나라나 영국인 선수정보 조회 customer table
select * from customer_table where addr like '대한민국%'or addr like'영국%';
-- 문제 14) 고객이 주문한 도서의 총 판매액 조회 orders_table
select custid as 주문자명,sum(saleprice) as 총판매액 from orders_table where saleprice group by custid ;
-- 문제 15) 2번 김연아 고객이 주문한 도서의 총 판매액 조회
select custid as 주문자명,sum(saleprice) as 총판매액 from orders_table where custid=2 and saleprice group by custid;
-- 문제 16) 고객이 주문한 도서의 총 판매액, 평균값,최저가,최고가 조회(문제에대해질문하기)
select bookid as 도서아이디,sum(saleprice) as 총판매액, avg(saleprice) as 평균판매액, min(saleprice) as 최저가, max(saleprice) as 최고가 from orders_table where saleprice group by bookid; 
-- 문제 17) 마당서점의 도서 판매 건수 조회(문제에대해질문하기)
select count(*) as 서점총판매 from orders_table where orderid;
-- 문제 18) 고객별로 주문한 도서의 총 수량과 총 판매액 조회
select custid as 고객아이디,count(custid),sum(saleprice) from orders_table where custid and saleprice group by custid;
-- 문제 19) 가격이 8000원 이상인 도서를 구매한 고객에 대하여 고객별 주문 도서의 총 수량을 구하시오. 단, 두 권 이상 구매한 고객만 조회
select custid as 고객아이디,count(custid) as 총구매한도서수량 from orders_table where saleprice >=8000 group by custid having 총구매한도서수량 >=2; 
-- 문제 20) 날짜별 총 구매건수와 총 판매액을 조회
select orderdate as 구매일,count(orderdate) as 구매건수,sum(saleprice)as 총판매액 from orders_table group by orderdate;
-- 문제 21) 총 판매액이 20000이 넘는 날짜의 총 구매건수를 조회
select orderdate as 구매일,count(orderdate) as 구매건수,sum(saleprice)as 총판매액 from orders_table group by orderdate having 총판매액 >20000;
-- 문제 22) 가장 구매건수가 많은 날짜를 조회 구매건수가 같은 경우 가장 최근 날짜를 조회
select orderdate as buydate from orders_table group by orderdate having count(*) =(select max(mycount) as 최대구매수 from (select orderdate as buydate, count(*) as mycount from orders_table  group by orderdate) as result) order by orderdate desc limit 1;