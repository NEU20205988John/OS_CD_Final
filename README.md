# OS_CD_Final
A small Linux-like operating system

Login:

![login](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/5156bb44-b418-41c5-8cd4-42ddd55e1eeb)


An admin can format the system:
![format1](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/3f58cb62-8cc2-4aba-ab94-ff768e50de7e)
![format2](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/227b4c9c-6ac2-4e0e-a243-bd5a64a4a2d2)

An admin can also add or delete a user:
![useradd](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/8e99dbbd-854d-4e6c-97db-907a674d2bf3)
![userdel](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/a9cf8efa-342b-4cbb-a744-c533886d2178)
![useradd_logout_login](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/727c69ca-7f9d-4b63-881f-aa358405cca0)


make a directory(mkdir):
![mkdir](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/dab06be6-ef52-440f-8642-d83b25cc246b)

see your current directory using ls(dir):
![ls](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/72bc6f17-ac1e-40fc-92bd-3445d2130d0f)


Then change your directory using cd(chdir):
![cd(chdir)](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/6b951dee-c1b2-4e2b-bbb4-37d3a2da2325)


create a file using "touch":
![touch(create)](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/58abaeb2-7c70-42da-9f89-faf7bf56b57d)

then open it:
![open](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/384a85ba-cc27-44e9-9521-46d3afd07c87)

You'll receive a file descriptor, it'll be used on file's closure, wirte, and read.

Let's try write sth into the file example.cpp
![write](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/a61ab6cb-6257-4e6a-924b-8130c433bfd8)

Then read it out!
![read](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/c99a5a58-73b6-49ab-a366-6f81c28ca303)

close it:
![close](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/ab1ac894-56a1-4183-bc70-675a3f2e1d83)

delete the file using rm(remove):
![delete(rm)](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/ccf77654-cfa2-476e-a670-150568e76183)

While the file is open, you can check the useropened table & the systemopened table:
![useropened_systemopened](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/da26272a-4e7f-4c66-a6ae-f71026855403)
After closing the file, you'll notice the difference.
![afterclose_useropened_systemopened](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/5a71dbf7-ab8b-4942-b150-ca4eb7711444)

Besides, you can check out the permission and edit user permit:
![editpermit_viewpermit](https://github.com/NEU20205988John/OS_CD_Final/assets/80146486/5ee3bc23-a1e4-49d6-9eb1-e90c843d0010)

