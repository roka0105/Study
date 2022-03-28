use cookdb;
select * from usertbl where height >=180 and height <=182;
select * from usertbl where height between 180 and 182;
select * from usertbl where addr ='경남'or addr='충남';
select * from usertbl where addr in ('경남','충남','경북');
select * from usertbl where addr not in ('경남','충남','경북');
select * from usertbl where addr != '경남';

select username as 이름,height as 키 from usertbl where username like '%김%';
select username as 이름,height as 키 from usertbl where username like '_경규';

use employees;
select * from employees where gender ='F' and birth_date between '1950-01-01' and '1959-12-31';
select * from employees where first_name like '%k%' and hire_date >= '1990-01-01';
select * from employees where hire_date >= '1990-01-01' and binary first_name like '%K%' limit 5;
select * from titles;
select * from titles where title in ('engineer','staff','senior staff') limit 5;
select * from titles where  binary title in ('Engineer','staff','Senior Staff'); 

select title as 타이틀 from titles;
select distinct title as 타이틀 from titles;

use cookdb;
select username,height from usertbl where height >= (select height from usertbl where username='김용만');
select * from usertbl where addr ='경기';
select * from usertbl where height >= any(select height from usertbl where addr='경기');
select * from usertbl where height >= all(select height from usertbl where addr='경기');

select * from usertbl where height order by height asc;
select * from usertbl order by height desc;
select * from usertbl order by username asc,height desc;

select distinct addr as 거주지 from usertbl;

use employees;
select * from employees order by hire_date asc;
select emp_no,hire_date from employees order by hire_date asc;
select emp_no,hire_date from employees order by hire_date asc limit 10,5;

use cookdb;
create table test01 (select userid,username,height from usertbl where height>=(select height from usertbl where username='김용만'));

select userid, sum(price* amount) from buytbl group by userid;
select userid, sum(price* amount) from buytbl group by userid order by sum(price*amount) desc;

select userid,sum(amount) as 구매개수 from buytbl group by userid order by sum(amount) desc;
select count(*) from usertbl;
select count(*) from usertbl where height >=180;
select addr,count(*) from usertbl group by addr order by addr asc;
select * from usertbl;
select addr,count(*) from usertbl where height between 180 and 190 group by addr;
-- where 절 부터 실행되기 때문에 집계함수는 제일 마지막에 사용됨.

use employees;
-- 회원정보에 f가 들어간 이름을 남녀 구분하여 카운트한 결과 출력
select first_name as 이름,gender as 성별  from employees where binary first_name ='%f%';
select first_name as 이름,gender as 성별,count(*) as 인원수 from employees where binary first_name like '%k%'or'%I%' group by gender order by first_name asc;
use cookdb;
-- max
select addr,max(height) from usertbl  group by addr;
-- avg
select addr,avg(height) from usertbl group by addr;
-- min
select addr,min(height) from usertbl group by addr;

-- 가장 큰 키의 유저 정보를 알고싶다.
select * from usertbl where height = (select max(height) from usertbl); 

-- 지역별로 가장 큰 사람의 정보 출력
select max(height) from usertbl group by addr;
select * from usertbl where height  in (select max(height) from usertbl group by addr) order by  height asc;

-- 총 구매액이 1000 넘는 유저 정보
select userid,sum(price*amount) as 총구매액 from buytbl group by userid having sum(price*amount)>1000;

-- 총 구매액이 1000원 넘는 사람들만 보기
select userid as 아이디,sum(price*amount) as 총결제액 from buytbl group by userid having sum(price*amount)>1000; 

--

