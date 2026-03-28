# ctools - My personal tools for C development

## Installation
This library is published from `https://sleiphan.github.io/debian-repo`.

```sh
# Add the package repository to your package manager
echo "deb [signed-by=/usr/share/keyrings/myrepo.gpg] https://sleiphan.github.io/debian-repo stable main" | tee /etc/apt/sources.list.d/myrepo.list

# Add the repository's public gpg key
curl -fsSL https://sleiphan.github.io/debian-repo/public.key | gpg --dearmor -o /usr/share/keyrings/myrepo.gpg

# Download the index of the new repository
apt update

# Install dev library
apt install libctools-dev

# Install runtime library
apt install libctools0
```
