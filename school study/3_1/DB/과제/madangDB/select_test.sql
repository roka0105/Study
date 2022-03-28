-- 데이터베이스가 이미 존재한다면 삭제한다. 
drop database if exists madang;
-- 데이터베이스 생성.
create database madang;
-- 지정한 데이터베이스 사용.
use madang;
-- book 테이블 생성. 
create table book_table
(bookid int auto_increment primary key,bookname varchar(20) not null,publisher varchar(20) not null,price int not null);

-- book 데이터 삽입. autoincrement 한 변수자리에는 null을 넣으면 자동으로 숫자가 증가해서 넣어줌.
insert into book_table values(null,'축구의 역사','굿스포츠',7000);
insert into book_table values(null,'축구아는 여자','나무수',13000);
insert into book_table values(null,'축구의 이해','대한미디어',22000);
insert into book_table values(null,'골프 바이블','대한미디어',35000);
insert into book_table values(null,'피겨교본','굿스포츠',8000);
insert into book_table values(null,'역도 단계별기술','굿스포츠',6000);
insert into book_table values(null,'야구의 추억','이상미디어',20000);
insert into book_table values(null,'야구를 부탁해','이상미디어',13000);
insert into book_table values(null,'올림픽 이야기','삼성당',7500);
insert into book_table values(null,'Olympic Champions','Pearson',13000);

-- cust 테이블 생성. 
create table customer_table
(custid int auto_increment primary key,name varchar(20) not null,address varchar(30) not null,phone char(13));

-- cust 데이터 삽입.
insert into customer_table values(null,'박지성','영국 맨체스타','000-5000-0001');
insert into customer_table values(null,'김연아','대한민국 서울','000-6000-0001');
insert into customer_table values(null,'장미란','대한민국 강원도','000-7000-0001');
insert into customer_table values(null,'추신수','미국 클리블랜드','000-8000-0001');
insert into customer_table values(null,'박세리','대한민국 대전',null);

-- orders 테이블 생성. 
create table orders_table
(orderid int auto_increment primary key,custid int not null,bookid int not null,saleprice int not null,orderdate date not null,
foreign key (custid) references customer_table(custid),foreign key(bookid)references book_table(bookid));
-- foreign key= 외래키 설정하는것 foreign key(현 테이블에서 외래키 지정할것의 이름) references 테이블명(테이블에서 가져올 키 이름);

-- orders 데이터 삽입.
insert into orders_table values(null,1,1,6000,'2014-07-01');
insert into orders_table values(null,1,3,21000,'2014-07-03');
insert into orders_table values(null,2,5,8000,'2014-07-03');
insert into orders_table values(null,3,6,6000,'2014-07-04');
insert into orders_table values(null,4,7,20000,'2014-07-05');
insert into orders_table values(null,1,2,12000,'2014-07-07');
insert into orders_table values(null,4,8,13000,'2014-07-07');
insert into orders_table values(null,3,10,12000,'2014-07-08');
insert into orders_table values(null,2,10,7000,'2014-07-09');
insert into orders_table values(null,3,8,13000,'2014-07-10');

-- 1. 가격이 20000원 미만인 도서를 검색
select * from book_table where price < 20000;

-- 2. 가격이 10000원 이상 20000원 이하 도서를 검색
-- 두가지 방법을 사용 할 수 있습니다. 범위값을 비교할 경우 between 또는 >= <= 을 사용합니다.
select * from book_table where price between 10000 and 20000;
select * from book_table where price >=10000 and price <=20000;

-- 3. 출판사가 '굿스포츠' 혹은 '대한미디어'인 도서를 검색
-- 두가지 방법을 사용 할 수 있습니다. in 사용시 괄호안에 들어간 내용중 하나라도 같은게 있다면 그 필드를 리턴합니다.다른방법으로는 = 연산으로 비교할 수 있습니다.
select * from book_table where publisher in('굿스포츠','대한미디어');
select * from book_table where publisher= '굿스포츠'or publisher='대한미디어';

-- 4. 출판사가 '굿스포츠' 혹은 '대한미디어'가 아닌 도서를 검색
-- 두가지 방법을 사용 할 수 있습니다. not in 으로 괄호안에 들어간 내용 모두가 아닌 경우의 필드만 리턴합니다. 다른방법으로는 != 연산으로 비교할 수 있습니다.
select * from book_table where publisher not in('굿스포츠','대한미디어');
select * from book_table where publisher != '굿스포츠' and publisher != '대한미디어';

-- 5. '축구의 역사'를 출간한 출판사를 검색
select * from book_table where bookname = '축구의 역사';

-- 6. 도서이름에 '축구'가 포홤된 출판사를 검색
-- like 로 와일드 문자(%,_)로 표현된 문자열과 비슷한 필드 발견시 리턴해 줍니다. 
select * from book_table where bookname like '%축구%';

-- 7. 도서 이름이 여섯 글자인 도서를 검색
-- _ 는 글자 아무 글자 1개를 표현합니다.
select * from book_table where bookname like '______';

-- 8. 도서 이름의 왼쪽 두번째 위치에 '구'라는 문자열을 갖는 도서를 검색
select * from book_table where bookname like '_구%';

-- 9. 축구에 관한 도서 중 가격이 20000원 이상인 도서를 검색 
select * from book_table where price >=20000;

-- 10. 야구에 관한 책을 모두 구입하려면 필요한 금액 계산
-- 집계합계함수 sum() 은 리턴된 필드의 합계를 구해주는 함수입니다.
select sum(price) from book_table where bookname like '%야구%'; 

-- 11. 도서를 가격 순으로 검색하고, 가격이 같으면 이름순으로 검색
-- order by 는 정렬을 위한 명령어 입니다. price 와 bookname중 price를 우선으로 하여 정렬하며 price가 같을 경우 bookname으로 정렬합니다.
-- asc는 오름차순 , desc 는 내림차순 입니다.
select * from book_table order by price asc,bookname asc;
select * from book_table order by price desc,bookname desc;

-- 12. 도서를 가격의 내림차순으로 검색하고 만약 가격이 같다면 출판사의 오름차순으로 검색.
select * from book_table order by price desc,publisher asc;

-- 13. 주소가 우리나라나 영국인 선수정보 조회
select * from customer_table where address like '대한민국%' or address like '영국%';

-- 14. 고객이 주문한 도서의 총 판매액 조회
-- group by는 어떤 필드 기준으로 하여 그룹지어 데이터를 표현할 것인지에 대한 명령어 입니다.
select custid as 고객명,sum(saleprice) as 총구매액 from orders_table group by custid; 

-- 15. 2번 김연아 고객이 주문한 도서의 총 판매액 조회
select sum(saleprice) as 총구매액 from orders_table where custid=2;

-- 16. 고객이 주문한 도서의 총 판매액, 평균값,최저가,최고가 조회
-- 집계함수 sum()필드의 합계 / avg()필드의 평균 min() / min() 필드의 최저값 / max() 필드의 최고값
select sum(saleprice) as 총판매액,avg(saleprice) as 평균값 ,min(saleprice) as 최저가,max(saleprice) as 최고가 from orders_table;

-- 17. 마당서점의 도서 판매 건수 조회
-- 집계함수 count(*)는 리턴되는 레코드당 1번의 카운트를 하여 총 몇개의 레코드가 있는지 알 수 있습니다.
select count(*) as 총판매도서건수 from orders_table;

-- 18. 고객별로 주문한 도서의 총 수량과 총 판매액 조회
select custid as 고객id,count(*) as 총수량,sum(saleprice) as 총판매액 from orders_table group by custid;

-- 19. 가격이 8000원 이상인 도서를 구매한 고객에 대하여 고객별 주문 도서의 총 수량을 구하시오. 단,두권 이상 구매한 고객만 조회.
-- having 은 집계함수와 사용이 가능합니다. where절 처럼 조건을 검사하는 명령어인데, 
-- where과 다른점은 where 은 맨 먼저 실행되고 where에서 리턴한 레코드값을 가지고 집계함수를 마지막에 처리하기 때문에
-- having을 사용합니다. having은 마지막에 실행되며 집계함수와 사용해야만 합니다.
select custid as id, count(*) as 주문수량 from orders_table where saleprice >= 8000 group by custid having count(*)>=2; 

-- 20. 날짜별 총 구매건수와 총 판매액을 조회
select orderdate as 주문일자,count(*) as 구매건수,sum(saleprice) as 총판매액 from orders_table group by orderdate;

-- 21. 총 판매액이 20000이 넘는 날짜의 총 구매건수를 조회
select orderdate as 주문일자,sum(saleprice) as 총판매액,count(*) as 구매건수 from orders_table group by orderdate having sum(saleprice)>=20000;

-- 22. 가장 구매건수가 많은 날짜를 조회 구매건수가 같은 경우 가장 최근 날짜를 조회
-- from 절 서브쿼리: from절은 테이블 명이 오도록 되어있습니다. 그런데 서브쿼리가 from 절에 사용되면 동적으로 생성된 테이블인 것 처럼 사용할 수 있습니다. 
--  질문
--  왜 from절 서브쿼리 이후 as 임의의이름 으로 변환시켜줘야 할까요(안하면 오류남)?
--  제 생각에는 from절은 테이블명이 오도록 하는 규칙때문에 서브쿼리에서 반환된 레코드를 as 로 테이블 명처럼 변환 시키는것이 아닐까 합니다.(이 부분 교수님께 질문하기)
--  근데 select ~~~~ 결과 값들은 테이블 아닌가요? 현재 저 부분에서 만족되는 14-07-03,14-07-07 두 레코드들을 담은 임시 테이블을 반환해주는것 같은데
--  임시 테이블에 대한 별칭을 붙여줘야 from절에서 사용할 수 있는 걸까요?

-- 코드 진행순서
-- 1) orderdate as 주문일자, count(*) as 주문건수 from orders_table group by orderdate 여기서 날짜별 주문 건수에 대한 테이블 목록을 가지고 있다.
-- 2) 날짜별 주문건수 count(*) 와 (select max(구매건수) from (select count(*) as 구매건수 from orders_table group by orderdate ) as result)의 리턴값이 같으면 해당 레코드를 가져온다.
-- 3)  from (select count(*) as 구매건수 from orders_table group by orderdate ) 이 서브쿼리에서 날짜별 주문건수를 지닌 테이블을 리턴하는데,
-- 4)  (select max(구매건수) from ( 서브쿼리로 리턴한 날짜별 주문건수 테이블 ) as result) 가져온 서브쿼리 테이블에서 max 최대치의 주문건수 레코드를 걸러낸다.
-- 5) 최대 주문건수 레코드를 걸러냈으니 최근 날짜부터 뜨도록 정렬한다 order by orderdate desc
-- 6) 제일 최신 날짜만 뜨도록 해야 하므로 limit;을 사용한다.  limit 기본값은 1이다.

-- limit 사용시 10, 5 이런식으로 사용한다면 테이블에서 11번째부터 5개를 읽어오겠다는 의미이다.
select orderdate as 주문일자, count(*) as 주문건수 from orders_table group by orderdate having count(*) = (select max(구매건수) from (select count(*) as 구매건수 from orders_table group by orderdate ) as result) order by orderdate desc limit 1;

