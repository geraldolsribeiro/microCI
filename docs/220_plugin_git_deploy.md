# 🚢 git_deploy plugin


## What it does

The `git_deploy` plugin checks out a repository into a deployment directory and exposes only the files needed for production.

## Why it exists

Use `git_deploy` when you want the deployment target to receive a clean snapshot of the latest commit without carrying extra repository history into the published directory.

## Example

```yaml
steps:
  - name: "Deploy application snapshot"
    plugin:
      name: "git_deploy"
      repo: "git@your-server.com:app_deploy.git"
      git_dir: "/opt/microCI/repos/app_deploy"
      work_tree: "/var/www/app"
      clean: true
```

## Notes

- Useful for static sites and release deployments.
- Separates repository history from the deployed files.
