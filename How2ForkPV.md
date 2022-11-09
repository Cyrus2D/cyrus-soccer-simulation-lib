# How2ForkPv

## Bare clone a public repo, and mirror push it to the private repo

Duplicate public to private repository
Create empty private repo

```bash
git clone --bare https://github.com/exampleuser/public_repo.git
cd public_repo.git
git push --mirror https://github.com/yourname/private_repo.git
cd ..
rm -rf public-repo.git
```

## Register remote repository with public repository

```bash
git clone https://github.com/yourname/private_repo.git
cd private_repo
git remote add public https://github.com/exampleuser/public_repo.git
```

## Fetching from public repo

```bash
git pull public master 
git push origin master
```

be careful of branches!
