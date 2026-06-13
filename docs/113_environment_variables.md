# Environment variables

The environment variables are loaded from the following files:

| File name            | Format       |
|----------------------|--------------|
| `$HOME/.microCI.env` | `VAR=VALUE`  |
| `.env`               | `VAR=VALUE`  |
| `.env.yml`           | `VAR: VALUE` |

Some special variables can be automatically loaded if defined.

## Format

### .env

```
# File .env
ENV_1=1
ENV_2=String with spaces 
```

### .env.yml

```
# File .env.yml
ENV_YML_1: 1
ENV_YML_2: String with spaces 
```


